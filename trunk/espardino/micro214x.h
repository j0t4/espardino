/***************************************************************/
/* Copyright (c) 2009 NBEE Embedded Systems S.L.               */
/* All rights reserved.                                        */
/*                                                             */
/* Permission is hereby granted, free of charge, to any        */
/* person obtaining a copy of this software and associated     */
/* documentation files (the "Software"), to deal in the        */
/* Software without restriction, including without limitation  */
/* the rights to use, copy, modify, merge, publish,            */
/* distribute, sublicense, and/or sell copies of the           */
/* Software, and to permit persons to whom the Software is     */
/* furnished to do so, subject to the following conditions:    */
/*                                                             */
/* This code is distributed only with and for Espardino(tm)    */
/* products or derivative final products not intended as       */
/* development systems.                                        */
/*                                                             */
/* The above copyright notice and this permission notice       */
/* shall be included in all copies or substantial portions of  */
/* the Software.                                               */
/*                                                             */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY   */
/* KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE  */
/* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR     */
/* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS  */
/* OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR    */
/* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR  */
/* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE   */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.      */
/***************************************************************/



#ifndef __MICRO214x_H
#define __MICRO214x_H

#define __ESPARDINO_LIB_VER  "1.00.03"

#include <micro214x_types.h>
#include <leds.h>
#include <efsutil.h>
#include <pwm.h>
#include <miniprintf.h>
#include <adc.h>
#include <pwm_in.h>
#include <pwm.h>
#include <i2c_device.h>
#include <arm_irqs.h>
#include <sysclocks.h>
#include <iolib.h>
#include <rtc.h>
#include <delay.h>

#ifdef __cplusplus

  typedef void(*ptVoidFunction)(void);

	#include <digital_io.h>
	#include <digital_bus.h>
	#include <pwm_c++.h>
	#include <ticker.h>
	#include <servo.hpp>
	#include <serial.h>
	
#endif
#endif

