
#ifndef __VCOM_H
#define __VCOM_H


#ifdef __cplusplus
extern "C" {
#endif
int VCOM_putchar_nonblock(int c);
int VCOM_putchar_nonblock_nl(int c);
int VCOM_getchar_nonblock(void);
int VCOM_kbhit(void);
int VCOM_putchar(int c);
int VCOM_putchar_nl(int c);
int VCOM_getchar(void);
int VCOM_init(void);   
void VCOM_puts(const char *str);

#ifdef __cplusplus
}
#endif



/* already documented on VCOM.h without realtime */
#endif 
