#!/bin/sh

### LINK OS ###
#arm-none-eabi-gcc build/obj/crt0.o build/obj/vectors.o build/obj/chcore.o build/obj/chcore_v7m.o build/obj/nvic.o build/obj/chsys.o build/obj/chdebug.o build/obj/chlists.o build/obj/chvt.o build/obj/chschd.o build/obj/chthreads.o build/obj/chdynamic.o build/obj/chregistry.o build/obj/chsem.o build/obj/chmtx.o build/obj/chcond.o build/obj/chevents.o build/obj/chmsg.o build/obj/chmboxes.o build/obj/chqueues.o build/obj/chmemcore.o build/obj/chheap.o build/obj/chmempools.o build/obj/hal.o build/obj/adc.o build/obj/can.o build/obj/ext.o build/obj/gpt.o build/obj/i2c.o build/obj/icu.o build/obj/mac.o build/obj/mmc_spi.o build/obj/mmcsd.o build/obj/pal.o build/obj/pwm.o build/obj/rtc.o build/obj/sdc.o build/obj/serial.o build/obj/serial_usb.o build/obj/spi.o build/obj/tm.o build/obj/uart.o build/obj/usb.o build/obj/stm32_dma.o build/obj/hal_lld.o build/obj/adc_lld.o build/obj/ext_lld_isr.o build/obj/can_lld.o build/obj/ext_lld.o build/obj/gpt_lld.o build/obj/icu_lld.o build/obj/mac_lld.o build/obj/pwm_lld.o build/obj/sdc_lld.o build/obj/pal_lld.o build/obj/i2c_lld.o build/obj/rtc_lld.o build/obj/spi_lld.o build/obj/serial_lld.o build/obj/uart_lld.o build/obj/usb_lld.o build/obj/board.o build/obj/syscalls.o build/obj/shell.o build/obj/chprintf.o build/obj/flash.o build/obj/helper.o build/obj/ihex.o build/obj/main.o build/obj/print.o build/obj/app1.o build/obj/app2.o build/obj/app3.o -mcpu=cortex-m3 -nostartfiles -T./STM32F103xB.ld -Wl,-Map=bl/os.map,--cref,--no-warn-mismatch -mno-thumb-interwork -mthumb -lm -o bl/os.elf

#COMPILE APPS
rm -fR bl/app*
cp build/obj/app*.o bl/

#LINK APPS
arm-none-eabi-ld bl/app1.o -Map=bl/app1.map --script=./STM32F103xB.ld --just-symbols=bl/os.elf --section-start=.text=0x08010440 -o bl/app1.elf
arm-none-eabi-ld bl/app2.o -Map=bl/app2.map --script=./STM32F103xB.ld --just-symbols=bl/os.elf --section-start=.text=0x08010000 -o bl/app2.elf
arm-none-eabi-ld bl/app3.o -Map=bl/app3.map --script=./STM32F103xB.ld --just-symbols=bl/os.elf --section-start=.text=0x08012000 -o bl/app3.elf

#OBJCOPY -> .hex
arm-none-eabi-objcopy -O ihex bl/app1.elf bl/app1.hex
arm-none-eabi-objcopy -O ihex bl/app2.elf bl/app2.hex
arm-none-eabi-objcopy -O ihex bl/app3.elf bl/app3.hex

