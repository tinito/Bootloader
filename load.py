#!/usr/bin/env python

import sys, os, io
import subprocess
import serial
from elftools.elf.elffile import ELFFile


SERIAL_PORT = '/dev/ttyUSB0'
#SERIAL_PORT = '/dev/pts/41'
SERIAL_BAUD = 115200
ENTRY_THREAD_NAME = 'app_thread'
THREAD_PC_OFFSET = 1
MAX_NAME_LENGTH = 31


def __find_adress(elffile, name):
    dwarfinfo = elffile.get_dwarf_info()
    for CU in dwarfinfo.iter_CUs():
        for DIE in CU.iter_DIEs():
            try:
                if DIE.tag == 'DW_TAG_subprogram' and DIE.attributes['DW_AT_name'].value == name:
                    return int(DIE.attributes['DW_AT_low_pc'].value) + THREAD_PC_OFFSET
            except KeyError: continue
    raise RuntimeError('Symbol "%s" not found' % name)


def __main():
    # Get the LD parameters
    if len(sys.argv) < 7:
        raise ValueError('Command line:\n\tappname hexpath stacklen ldexe ldscript ldoself ldmap ldobjelf ldobj*\nGiven:\n\t' + str(sys.argv))
    appname  = sys.argv[1].strip()[:MAX_NAME_LENGTH]
    hexpath  = sys.argv[2]
    stacklen = int(sys.argv[3][3:], 16) if sys.argv[3].strip().lower()[0:2] == '0x' else int(sys.argv[3])
    ldexe    = sys.argv[4]
    ldscript = sys.argv[5]
    ldoself  = sys.argv[6]
    ldmap    = sys.argv[7]
    ldobjelf = sys.argv[8]
    ldobjs   = sys.argv[9:]
    
    # Get the length of each section
    pgmlen, bsslen, datalen = 0, 0, 0
    with open(ldobjelf, 'rb') as f:
        elffile = ELFFile(f)
        for section in elffile.iter_sections():
            if   section.name == '.text':   pgmlen  = int(section['sh_size'])
            elif section.name == '.bss':    bsslen  = int(section['sh_size'])
            elif section.name == '.data':   datalen = int(section['sh_size'])
    
    # Open the serial port
    ser = serial.Serial(SERIAL_PORT, SERIAL_BAUD)
    sio = io.TextIOWrapper(buffer = io.BufferedRWPair(ser, ser, 1),
                           newline = '\r\n',
                           line_buffering = True,
                           encoding = 'ascii')

    try:
        # Send the section sizes and app name
        sizestr = '%0.8X,%0.8X,%0.8X,%0.8X,%0.2X%s' % (pgmlen, bsslen, datalen, stacklen, len(appname), appname)
        print sizestr
        sio.write(unicode(sizestr + '\n'))

        # Receive the allocated addresses
        addrs = sio.readline().split(',')
        pgmadr     = int(addrs[0].strip(), 16)
        bssadr     = int(addrs[1].strip(), 16)
        dataadr    = int(addrs[2].strip(), 16)
        datapgmadr = int(addrs[2].strip(), 16)
        
        # Link to the OS symbols
        sectopt = [ '--section-start', '.text=0x%0.8X' % pgmadr,
                    '--section-start', '.bss=0x%0.8X' % bssadr,
                    '--section-start', '.data=0x%0.8X' % dataadr ]
        args = [ ldexe, '--script', ldscript, '--just-symbols', ldoself, '-Map', ldmap, '-o', ldobjelf ] + sectopt + ldobjs
        print args
        subprocess.call(args)
        subprocess.call(['make'])
        
        with open(ldobjelf, 'rb') as f:
            elffile = ELFFile(f)
            threadadr = __find_adress(elffile, ENTRY_THREAD_NAME)
            print 'app_thread = 0x%0.8X' % threadadr
        
        # Read the generated IHEX file and remove unused records
        with open(hexpath, 'r') as f:
            hexdata = f.readlines()
        hexdata = [ line.strip() for line in hexdata if not line[7:9] in [ '05', '03' ] and len(line) >= 11 ]
        hexdata = [ line for line in hexdata if len(line) > 0 ]
        if len([ None for line in hexdata if line[7:9] == '01' ]) != 1 and hexdata[-1][7:9] != '01':
            raise RuntimeError('The IHEX must contain a single EOF record, as last record')
        
        # Insert the entry point thread record
        chks = threadadr & 0xFFFFFFFF
        chks = (chks >> 24) + (chks >> 16) + (chks >> 8) + (chks & 0xFF)
        chks = 0x100 - (0x04 + 0x00 + 0x00 + 0x05 + chks) & 0xFF
        hexdata[0:0] = [ ':04000005%0.8X%0.2X' % (threadadr, chks) ]
        
        # Send IHEX records
        for i in range(len(hexdata)):
            line = sio.readline().strip()
            print line
            if line != ',':
                raise RuntimeError('Error while loading line %d ("%s", received "%s")' % (i, hexdata[i], line))
            sio.write(unicode(hexdata[i] + '\n'))
            print hexdata[i]
        
        line = sio.readline().strip()
        print line
        if line != '^':
            raise RuntimeError('Error while terminating programming (received "%s")' % line)
        ser.close()
        
    except:
        print '!'
        sio.write(unicode('\n::::::::\n!\n'))
        ser.close()
        raise


if __name__ == '__main__':
    __main()

