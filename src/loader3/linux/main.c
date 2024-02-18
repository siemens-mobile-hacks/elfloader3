#define __linux_impl__
#include <inc/swilib.h>
#include <stdio.h>

#include "../loader.h"
#include "../../config_struct.h"

int main(int argc, char **argv) {
	config->loader_warnings = 1;
	config->max_log_size = 50 * 1024;
	config->realtime_libclean = 1;
	
	elfloader_setenv("LD_LIBRARY_PATH", "../../../sdk/lib/NSG/;../../../sdk/lib/;../../../sdk/lib/legacy/", 1);
	
	if (argc < 2) {
		fprintf(stderr, "usage: %s file.elf\n", argv[0]);
		return -1;
	}
	
	const char *elf_path = argv[1];
	
	Elf32_Exec *ex = elfopen(elf_path);
	if (!ex) {
		fprintf(stderr, "elfopen(%s) error!\n", elf_path);
		return -1;
	}
	
	void (*addr)(const char *argc, char *argv) = (void(*)(const char* argc, char *argv)) elf_entry(ex);
	printf("Entry point: %p\n", addr);
	
	if (!addr) {
		fprintf(stderr, "elf_entry(%s) error!\n", elf_path);
		return -1;
	}
	
	run_INIT_Array(ex);
	run_FINI_Array(ex);
	
	elfclose(ex);
	
	printf("OK: %s\n", elf_path);
	
	return 0;
}
