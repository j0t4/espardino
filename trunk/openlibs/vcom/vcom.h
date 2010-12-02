
#ifndef __VCOM_H
#define __VCOM_H


#ifdef __cplusplus
extern "C" {
#endif
int VCOM_putchar_nonblock(int c);
int VCOM_putchar_nonblock_nl(int c);
int VCOM_getchar_nonblock(void);
int VCOM_putchar(int c);
int VCOM_putchar_nl(int c);
int VCOM_getchar(void);
int VCOM_kbhit(void);
int VCOM_init(void);  
void VCOM_puts(const char *str);

#ifdef __cplusplus
}
#endif


/*==FILE lib/virtual_com_port.txt */

/*==INTRO

The Virtual COM Port is based on the lpcusb libraries available on internet. They are BSD licensed, so you can use it even for your commercial projects, modify it, etc..
   
You can setup a virtual serial port with the USB host (your computer), an easy to build and effective communication method for most applications.

*/

/*==RESOURCES VIC Interrupt level 0, USB Interface.  */


/*==USAGE Using the virtual com port in your applications
    
1st) Edit your makefile to include the **"vcom"** library at linking time. You can modify the espardino base projects adding "-lvcom" to the MY_LIBS var.

<code c>
  # System libraries to be linked with
  MY_LIBS = -lvcom 
</code>
	
2nd) Include the "vcom.h" headers in your application.

<code c>
  #include <vcom.h>
</code>

3rd) Call VCOM_init() inside your application, during startup.It will setup all the VIC  interrupt vectors and underlying software to emulate a virtual COM port over the USB connection.

<code c>
  VCOM_init();	// init the USB Virtual COM port 
</code>

*/

/*==SECTION Sending and receiving data over the Virtual COM port (VCP) */

/*==FUN int VCOM_putchar_nonblock(int c) ===================================================
==ACTION
It writes a character on the VCP, if the output buffer is full it won't wait to send the character, returning a <0 value.

==RESULT
<0 value when buffer is full.
*/

/*==FUN int VCOM_putchar_nonblock_nl(int c) ===================================================
==ACTION
The same as VCOM_putchar_nonblock, but adding a carriage return \r for every \n newline found.

==RESULT
<0 value when buffer is full.
*/



/*==FUN int VCOM_getchar_nonblock(void) ==================================================
==ACTION
It reads a character on the VCP, if the input buffer is empty it won't 	wait for available data on the buffer,
returning a <0 value in this case.

==RESULT
Character from VCP input buffer, or <0 if buffer is empty 
*/

/*

/*==FUN int VCOM_putchar(int c) ==================================================
==ACTION
It writes a character on the VCP output buffer, in case the buffer is 	full it will wait until it has available space, blocking your application.
*/


/*==FUN int VCOM_putchar_nl(int c) ==================================================
==ACTION
The same as VCOM_putchar, but adding a carriage return \r for every \n newline found.
*/


/*==FUN int VCOM_getchar(void) ==================================================       
==ACTION
It reads a character from the VCP, if the VCP buffer hasn't available 	data it will wait.

==RESULT
Character from VCP input buffer.

*/

/*==FUN int VCOM_kbhit(void) ==================================================       
==ACTION
It returns the number of available bytes to be read in the VCP buffer.

==RESULT
number of bytes available for read.

*/

/*==FUN void VCOM_puts(const char *str) ==================================================
==ACTION
It writes an ASCIIZ string to the virtual COM port, blocking when it 	has not available space on the output buffer.

*/

/*==SECTION Using printf with the VCP library

Espardino libraries include an small footprint version of the printf/sprintf.  It's a lightweight implementation of the printf function, supporting %c, %s, %d, %u, %x and %X.

To setup the printf default output just call "printf_output" with a pointer to the desired char output function. In this example printf will be blocking on the VCP output buffer.

<code c>
   printf_output(&VCOM_putchar);
</code>

*/

#endif 
