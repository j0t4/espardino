#ifndef __port_espardino__H
#define __port_espardino__H

#include <ff.h>

#define MY_STDIN ((MY_FILE *)0xFFFF0000)
#define MY_STDOUT ((MY_FILE *)0xFFFF0001)
#define MY_STDERR ((MY_FILE *)0xFFFF0002)

typedef unsigned int  MY_FILE;
int my_exit(int val);
void lua_setup_fatfs(FATFS *fs);
int my_feof(MY_FILE *f);
int my_getc(MY_FILE *f);
int my_ungetc(int c, MY_FILE *f);
MY_FILE *my_fopen(char *filename,char* mode);
MY_FILE *my_freopen(char *nombre, char *modo, MY_FILE *stream);
int my_fclose(MY_FILE *f);
int my_fflush(MY_FILE *f);
int my_ftell(MY_FILE *f);
int my_fwrite(char *buffer, int tblock, int nblock, MY_FILE *f);
int my_fread(char *buffer, int tblock ,int nblock,MY_FILE *f);
char* my_fgets(char *buff,int len,MY_FILE *fil);
int my_fputs(char *buffer,MY_FILE *f);
int my_fputc(int c, MY_FILE *f);
int my_fseek(MY_FILE *f, long int offset, int mode);
int my_putchar(int c);
int my_fprintF(MY_FILE *f, const char *format, ...);
int my_printF(const char *format, ...);
int my_sprintF(char *out, const char *format, ...);

#endif

