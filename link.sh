#!/bin/sh

#OS
/home/tino/ChibiStudio/tools/GNU\ Tools\ ARM\ Embedded/4.6\ 2012q2/bin/arm-none-eabi-ld build/obj/crt0.o build/obj/vectors.o build/obj/chcore.o build/obj/chcore_v7m.o build/obj/nvic.o build/obj/chsys.o build/obj/chdebug.o build/obj/chlists.o build/obj/chvt.o build/obj/chschd.o build/obj/chthreads.o build/obj/chsem.o build/obj/chevents.o build/obj/chqueues.o build/obj/chmemcore.o build/obj/hal.o build/obj/gpt.o build/obj/pal.o build/obj/serial.o build/obj/stm32_dma.o build/obj/hal_lld.o build/obj/gpt_lld.o build/obj/serial_lld.o build/obj/pal_lld.o build/obj/board.o build/obj/syscalls.o build/obj/println.o build/obj/main.o "/home/tino/ChibiStudio/tools/GNU Tools ARM Embedded/4.6 2012q2/bin/../lib/gcc/arm-none-eabi/4.6.2/../../../../arm-none-eabi/lib/armv7-m/libc.a" -Map=bl/os.map -nostartfiles --script=./STM32F103xB.ld --cref --no-warn-mismatch -q -o bl/os.elf

#APP
/home/tino/ChibiStudio/tools/GNU\ Tools\ ARM\ Embedded/4.6\ 2012q2/bin/arm-none-eabi-ld -Map=bl/app.map --script=./STM32F103xB.ld build/obj/app.o --just-symbols=bl/os.elf -o bl/app.elf

#OBJCOPY -> .bin
/home/tino/ChibiStudio/tools/GNU\ Tools\ ARM\ Embedded/4.6\ 2012q2/bin/arm-none-eabi-objcopy -O binary bl/os.elf bl/os.bin
/home/tino/ChibiStudio/tools/GNU\ Tools\ ARM\ Embedded/4.6\ 2012q2/bin/arm-none-eabi-objcopy -O binary bl/app.elf bl/app.bin
