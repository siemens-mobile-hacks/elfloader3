#define __linux_impl__
#include <inc/swilib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../../config_struct.h"

uint32_t *pLIB_TOP = NULL;
uint32_t Library[0x1000] = {
	[0x0148] = (uint32_t) ShowMSG
};

config_structure_t _config = { 0 };

config_structure_t *config = &_config;

int __e_div(int a, int b) {
	return b % a;
}

int get_file_size(const char *file) {
    struct stat st;
	int ret = lstat(file, &st) == 0 ? st.st_size : -1;
    printf("get_file_size(%s) = %d\n", file, ret);
	return ret;
}

int __swi_fopen(const char *file, uint32_t f, uint32_t m, uint32_t *err) {
	int flags = 0;
	mode_t mode = S_IRUSR | S_IWUSR;
	
	if ((f & A_ReadOnly))
		flags |= O_RDONLY;
	
	if ((f & A_WriteOnly))
		flags |= O_WRONLY;
	
	if ((f & A_ReadWrite))
		flags |= O_RDWR;
	
	if ((f & A_Create))
		flags |= O_CREAT;
	
	if ((f & A_Exclusive))
		flags |= O_EXCL;
	
	if ((f & A_Truncate))
		flags |= O_TRUNC;
	
	if ((f & A_Append))
		flags |= O_APPEND;
	
	int ret = open(file, flags, mode);
	
	if (err)
		*err = ret < 0 ? errno : 0;
	
	fprintf(stderr, "__swi_fopen(%s, %d, %d, %p) = %d\n", file, flags, mode, err, ret);
	return ret;
}

int __swi_fread(int fd, void *data, size_t sz, uint32_t *err) {
	int ret = read(fd, data, sz);
	if (err)
		*err = ret < 0 ? errno : 0;
	return ret;
}

int __swi_fwrite(int fd, const void *data, size_t sz, uint32_t *err) {
	int ret = write(fd, data, sz);
	if (err)
		*err = ret < 0 ? errno : 0;
	return ret;
}

int __swi_setfilesize(int fd, size_t new_size, uint32_t *err) {
	int ret = ftruncate(fd, new_size);
	if (err)
		*err = ret < 0 ? errno : 0;
	return ret;
}

int __swi_fclose(int fd, uint32_t *err) {
	int ret = close(fd);
	if (err)
		*err = ret < 0 ? errno : 0;
	return ret;
}

int __swi_lseek(int fd, uint32_t offset, uint32_t origin, uint32_t *err, uint32_t *err2) {
	int ret = lseek(fd, offset, origin);
	if (err)
		*err = ret < 0 ? errno : 0;
	if (err2)
		*err2 = ret < 0 ? errno : 0;
	return ret;
}

void SUBPROC(void *, ...) {
	printf("SUBPROC called!\n");
}

void GetDateTime(TDate *, TTime *) {
	
}

void zeromem_a(void *d, int l) {
	memset(d, 0, l);
}

void ShowMSG(int a, int b) {
	printf("ShowMSG[%d]: %s\n", a, (const char *) b);
}

void l_msg(int a, int b) {
	ShowMSG(a, b);
}
