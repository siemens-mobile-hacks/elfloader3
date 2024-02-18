all:
	gcc -static -g -m32 -O0 -Wall -Wextra -Wno-unused-parameter -Wno-nonnull-compare -Werror -Ilinux -D__linux__ *.c linux/*.c -o elfloader-linux

clean:
	rm elfloader-linux

test:
	find /tmp/elfs/ -iname '*.elf' | wc -l
	find /tmp/elfs/ -iname '*.elf' -exec ./elfloader-linux {} \; 2>&1 | grep OK -i | wc -l

.PHONY: all clean
