#!/bin/sh

#LINK OS
arm-none-eabi-ld build/obj/crt0.o build/obj/vectors.o build/obj/chcore.o build/obj/chcore_v7m.o build/obj/nvic.o build/obj/chsys.o build/obj/chdebug.o build/obj/chlists.o build/obj/chvt.o build/obj/chschd.o build/obj/chthreads.o build/obj/chsem.o build/obj/chevents.o build/obj/chqueues.o build/obj/chmemcore.o build/obj/hal.o build/obj/gpt.o build/obj/pal.o build/obj/serial.o build/obj/stm32_dma.o build/obj/hal_lld.o build/obj/gpt_lld.o build/obj/serial_lld.o build/obj/pal_lld.o build/obj/board.o build/obj/syscalls.o build/obj/print.o build/obj/main.o "/home/tino/ChibiStudio/tools/GNU Tools ARM Embedded/4.6 2012q2/arm-none-eabi/lib/armv7-m/libc.a" build/obj/helper.o build/obj/flash.o build/obj/ihex.o -r -o bl/os.o

arm-none-eabi-ld bl/os.o  "/home/tino/ChibiStudio/tools/GNU Tools ARM Embedded/4.6 2012q2/arm-none-eabi/lib/armv7-m/libc.a" -Map=bl/os.map -nostartfiles --script=./STM32F103xB-os.ld --cref --no-warn-mismatch -q -o bl/os.elf

#COMPILE APPS
rm bl/app*
arm-none-eabi-gcc -c -mcpu=cortex-m3 -O0 -ffunction-sections -fdata-sections -fno-common  -Wall -Wextra -Wstrict-prototypes -Wa,-alms=build/lst/app1.lst -Dstrcasecmp=strcmp -DPORT_INT_REQUIRED_STACK=32  -DTHUMB_PRESENT -mno-thumb-interwork -DTHUMB_NO_INTERWORKING -MD -MP -MF .dep/app1.o.d -mthumb -DTHUMB -I. -I/opt/ChibiStudio/ChibiOS-git/os/ports/common/ARMCMx/CMSIS/include -I/opt/ChibiStudio/ChibiOS-git/os/ports/common/ARMCMx -I/opt/ChibiStudio/ChibiOS-git/os/ports/GCC/ARMCMx -I/opt/ChibiStudio/ChibiOS-git/os/ports/GCC/ARMCMx/STM32F1xx -I/opt/ChibiStudio/ChibiOS-git/os/kernel/include -I/opt/ChibiStudio/ChibiOS-git/os/hal/include -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32F1xx -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/GPIOv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/I2Cv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/RTCv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/SPIv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/USARTv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/USBv1 -I. -I/opt/ChibiStudio/ChibiOS-git/os/various -I../ apps/app1.c -o bl/app1.o

arm-none-eabi-gcc -c -mcpu=cortex-m3 -O0 -ffunction-sections -fdata-sections -fno-common  -Wall -Wextra -Wstrict-prototypes -Wa,-alms=build/lst/app2.lst -Dstrcasecmp=strcmp -DPORT_INT_REQUIRED_STACK=32  -DTHUMB_PRESENT -mno-thumb-interwork -DTHUMB_NO_INTERWORKING -MD -MP -MF .dep/app2.o.d -mthumb -DTHUMB -I. -I/opt/ChibiStudio/ChibiOS-git/os/ports/common/ARMCMx/CMSIS/include -I/opt/ChibiStudio/ChibiOS-git/os/ports/common/ARMCMx -I/opt/ChibiStudio/ChibiOS-git/os/ports/GCC/ARMCMx -I/opt/ChibiStudio/ChibiOS-git/os/ports/GCC/ARMCMx/STM32F1xx -I/opt/ChibiStudio/ChibiOS-git/os/kernel/include -I/opt/ChibiStudio/ChibiOS-git/os/hal/include -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32F1xx -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/GPIOv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/I2Cv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/RTCv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/SPIv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/USARTv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/USBv1 -I. -I/opt/ChibiStudio/ChibiOS-git/os/various -I../ apps/app2.c -o bl/app2.o

arm-none-eabi-gcc -c -mcpu=cortex-m3 -O0 -ffunction-sections -fdata-sections -fno-common  -Wall -Wextra -Wstrict-prototypes -Wa,-alms=build/lst/app3.lst -Dstrcasecmp=strcmp -DPORT_INT_REQUIRED_STACK=32  -DTHUMB_PRESENT -mno-thumb-interwork -DTHUMB_NO_INTERWORKING -MD -MP -MF .dep/app3.o.d -mthumb -DTHUMB -I. -I/opt/ChibiStudio/ChibiOS-git/os/ports/common/ARMCMx/CMSIS/include -I/opt/ChibiStudio/ChibiOS-git/os/ports/common/ARMCMx -I/opt/ChibiStudio/ChibiOS-git/os/ports/GCC/ARMCMx -I/opt/ChibiStudio/ChibiOS-git/os/ports/GCC/ARMCMx/STM32F1xx -I/opt/ChibiStudio/ChibiOS-git/os/kernel/include -I/opt/ChibiStudio/ChibiOS-git/os/hal/include -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32F1xx -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/GPIOv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/I2Cv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/RTCv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/SPIv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/USARTv1 -I/opt/ChibiStudio/ChibiOS-git/os/hal/platforms/STM32/USBv1 -I. -I/opt/ChibiStudio/ChibiOS-git/os/various -I../ apps/app3.c -o bl/app3.o

#LINK APPS
#arm-none-eabi-ld build/obj/app1.o -r -o bl/app1.o
#arm-none-eabi-ld build/obj/app2.o -r -o bl/app2.o
#arm-none-eabi-ld build/obj/app3.o -r -o bl/app3.o

#/home/tino/ChibiStudio/tools/GNU\ Tools\ ARM\ Embedded/4.6\ 2012q2/bin/arm-none-eabi-ld bl/os.o -Map=bl/os.map -nostartfiles --script=./STM32F103xB-bl.ld --cref --no-warn-mismatch -q -o bl/os.elf
#/home/tino/ChibiStudio/tools/GNU\ Tools\ ARM\ Embedded/4.6\ 2012q2/bin/arm-none-eabi-ld bl/app.o -Map=bl/app.map --script=./STM32F103xB-bl.ld --just-symbols=bl/os.elf -o bl/app.elf


arm-none-eabi-ld -Map=bl/app1.map --script=./STM32F103xB-app.ld --just-symbols=bl/os.elf -o bl/app1.elf bl/app1.o
arm-none-eabi-ld -Map=bl/app2.map --script=./STM32F103xB-app.ld --just-symbols=bl/os.elf -o bl/app2.elf bl/app2.o
arm-none-eabi-ld -Map=bl/app3.map --script=./STM32F103xB-app.ld --just-symbols=bl/os.elf -o bl/app3.elf bl/app3.o

#OBJCOPY -> .bin
arm-none-eabi-objcopy -O binary bl/os.elf bl/os.bin
#/home/tino/ChibiStudio/tools/GNU\ Tools\ ARM\ Embedded/4.6\ 2012q2/bin/arm-none-eabi-objcopy -O binary bl/app1.elf bl/app1.bin
arm-none-eabi-objcopy -O ihex bl/app1.elf bl/app1.hex
arm-none-eabi-objcopy -O ihex bl/app2.elf bl/app2.hex
arm-none-eabi-objcopy -O ihex bl/app3.elf bl/app3.hex




