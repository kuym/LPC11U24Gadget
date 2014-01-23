#!/bin/bash

if [ -f "toolpath" ]; then
	toolPath=$(cat toolpath)
	PATH="$toolPath:$PATH"
fi

sources="\
LPC11U00.cpp \
LPC11U00API.cpp \
libc.S \
libc.cpp \
main.cpp \
"

linkerFile="LPC11U24.x"

mkdir -p obj
rm obj/*

objs=""
count=0
for s in $sources; do
	ext=$(echo $s | sed -E 's/^.*\.([a-zA-Z]+)$/\1/')
	case $ext in
		c )
		lang="c -std=c99 -include lpc.h"
			;;
		cpp )
		lang="c++ -include lpc.h"
			;;
		s )
		lang=assembler
			;;
		S )
		lang=assembler-with-cpp
			;;
	esac
	f=$(basename $s | sed -E 's/(^.*)\.[a-zA-Z]+$/\1/')
	o="obj/$count-$f.o"
	count=$(($count + 1))

	echo "Building $s"
	arm-none-eabi-gcc -nostdlib -nodefaultlibs -fno-exceptions \
		-g -Os -mthumb -march=armv6-m -mcpu=cortex-m0 -Wno-attributes \
		-D__LPC11UXX__ -DUSB_DEVICE_ONLY \
		-I . -I lib/CDL/CMSISv2p00_LPC11Uxx/lpcinc -I lib/CDL/CMSISv2p00_LPC11Uxx/inc \
		-I lib/CDL/LPC11UxxLib/inc -I lib/LPCUSBLib/Drivers/USB \
		-o $o -x $lang -c $s

		if [ $? -ne 0 ]; then
			exit -1
		fi
	objs="$objs $o"
done

echo Linking...
arm-none-eabi-gcc -nostdlib -nodefaultlibs -fno-exceptions \
		-g -Os -mthumb -march=armv6-m -mcpu=cortex-m0 \
		-T $linkerFile -o obj/test.elf $objs

if [ $? == 0 ]; then
	
	arm-none-eabi-objcopy -j .text -j .data -O binary obj/test.elf obj/unsigned.bin
	/usr/local/bin/node cortex-checksum.js < obj/unsigned.bin > obj/test.bin
	arm-none-eabi-objdump -d obj/test.elf > obj/test.elf.disasm.txt

	#echo vectors:
	#dd if=obj/test.bin bs=32 count=1 | hexdump -C
fi
