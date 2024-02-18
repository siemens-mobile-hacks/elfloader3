all:
	gcc -static -g -m32 -O0 -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter -Wno-nonnull-compare -Werror -Ilinux -D__linux__ *.c linux/*.c -o elfloader-linux

clean:
	rm elfloader-linux

test:
	@echo "EP2"
	@find ../../../test-data/ep2/*.elf | wc -l
	@find ../../../test-data/ep2/*.elf -exec ./elfloader-linux {} \; 2>&1 | grep OK -i | wc -l
	@echo "EP3"
	@find ../../../test-data/*.elf | wc -l
	@find ../../../test-data/*.elf -exec ./elfloader-linux {} \; 2>&1 | grep OK -i | wc -l
.PHONY: all clean
