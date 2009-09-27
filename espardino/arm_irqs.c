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


#include "arm_irqs.h"


static inline unsigned __get_cpsr(void)
{
	unsigned long res;
	asm volatile (" mrs %0, cpsr" : "=r" (res) : /* without inputs */);
	return res;
}

static inline void __set_cpsr(unsigned cspr_val)
{
	asm volatile (" msr cpsr, %0" : /* no outputs */ : "r" (cspr_val) );
}

unsigned IRQ_disable(void)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr(_cpsr_val | CPSR_IRQ_MASK);
	return _cpsr_val;
}

unsigned IRQ_restore(unsigned old_cpsr)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr((_cpsr_val & ~CPSR_IRQ_MASK) | (old_cpsr & CPSR_IRQ_MASK));
	return _cpsr_val;
}

unsigned IRQ_enable(void)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr(_cpsr_val & ~CPSR_IRQ_MASK);
	return _cpsr_val;
}

unsigned FIQ_disable(void)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr(_cpsr_val | CPSR_FIQ_MASK);
	return _cpsr_val;
}

unsigned FIQ_restore(unsigned old_cpsr)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr((_cpsr_val & ~CPSR_FIQ_MASK) | (old_cpsr & CPSR_FIQ_MASK));
	return _cpsr_val;
}

unsigned FIQ_enable(void)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr(_cpsr_val & ~CPSR_FIQ_MASK);
	return _cpsr_val;
}



unsigned INT_disable(void)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr(_cpsr_val | CPSR_INT_MASK);
	return _cpsr_val;
}

unsigned INT_restore(unsigned old_cpsr)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr((_cpsr_val & ~CPSR_INT_MASK) | (old_cpsr & CPSR_INT_MASK));
	return _cpsr_val;
}

unsigned INT_enable(void)
{
	unsigned _cpsr_val;

	_cpsr_val = __get_cpsr();
	__set_cpsr(_cpsr_val & ~CPSR_INT_MASK);
	return _cpsr_val;
}

