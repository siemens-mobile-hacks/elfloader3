/* Stub for linux */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	unsigned int year;
	unsigned char month;
	unsigned char day;
} TDate;

typedef struct {
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
	unsigned int millisec;
} TTime;

// Seek
#define S_SET 0
#define S_CUR 1
#define S_END 2

// Permissions
#define P_WRITE 0x100
#define P_READ 0x80

// Access
#define A_ReadOnly 0
#define A_WriteOnly 1
#define A_ReadWrite 2
#define A_NoShare 4
#define A_Append 8
#define A_Exclusive 0x10
#define A_MMCStream 0x20
#define A_Create 0x100
#define A_Truncate 0x200
#define A_FailCreateOnExist 0x400
#define A_FailOnReopen 0x800

#define A_TXT 0x4000
#define A_BIN 0x8000

#define __thumb
#define __arm

#ifndef __linux_impl__
	#define fopen __swi_fopen
	#define fclose __swi_fclose
	#define fwrite __swi_fwrite
	#define fread __swi_fread
	#define lseek __swi_lseek
	#define setfilesize __swi_setfilesize
	#define mfree free
	
	#define setenv elfloader_setenv
	#define unsetenv elfloader_unsetenv
	#define getenv elfloader_getenv
	#define clearenv elfloader_clearenv
#else
	int elfloader_setenv(const char *name, const char *value, int replace);
	int elfloader_unsetenv(const char *name);
	char *elfloader_getenv(const char *var);
	int elfloader_clearenv(void);
#endif

void ShowMSG(int a, int b);

void zeromem_a(void *d, int l);

int __swi_fopen(const char *file, uint32_t f, uint32_t m, uint32_t *err);
int __swi_setfilesize(int fd, size_t new_size, uint32_t *err);
int __swi_fread(int fd, void *data, size_t sz, uint32_t *err);
int __swi_fwrite(int fd, const void *data, size_t sz, uint32_t *err);
int __swi_fclose(int fd, uint32_t *err);
int __swi_lseek(int fd, uint32_t offset, uint32_t origin, uint32_t *err, uint32_t *err2);

void SUBPROC(void *, ...);

void GetDateTime(TDate *, TTime *);
