/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * ST32F103xB memory setup.
 */
__main_stack_size__     = 0x0400;
__process_stack_size__  = 0x0400;

MEMORY
{
    flash : org = 0x08000000, len = 64k
    appcfg : org = 0x08000000+64k, len = 4k
    app : org = 0x08000000+68k, len = 60k
    ram : org = 0x20000000, len = 20k
}

__ram_start__           = ORIGIN(ram);
__ram_size__            = LENGTH(ram);
__ram_end__             = __ram_start__ + __ram_size__;

SECTIONS
{
    . = 0;
    _text = .;
    
    .appcfg : ALIGN(16) SUBALIGN(16)
    {
        bl/app.o (.appcfg)
        bl/app.o (.appcfg.*)
    } > appcfg
    
    .app : ALIGN(16) SUBALIGN(16)
    {
        bl/app.o (.text.startup.*)
        bl/app.o (.text)
        bl/app.o (.text.*)
        bl/app.o (.rodata)
        bl/app.o (.rodata.*)
        bl/app.o (.glue_7t)
        bl/app.o (.glue_7)
        bl/app.o (.gcc*)
    } > app
}

PROVIDE(end = .);
_end            = .;

__heap_base__   = _end;
__heap_end__    = __ram_end__;
