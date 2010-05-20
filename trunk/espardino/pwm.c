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


#include "sysclocks.h"

#include <LPC214x.h>


#define PWM_IR            (*((volatile unsigned int *) (0xe0014000)))
#define PWM_TCR           (*((volatile unsigned int *) (0xe0014004)))
#define PWM_TC            (*((volatile unsigned int *) (0xe0014008)))
#define PWM_PR            (*((volatile unsigned int *) (0xe001400c)))
#define PWM_PC            (*((volatile unsigned int *) (0xe0014010)))
#define PWM_MCR           (*((volatile unsigned int *) (0xe0014014)))
#define PWM_MR0           (*((volatile unsigned int *) (0xe0014018)))
#define PWM_MR1           (*((volatile unsigned int *) (0xe001401c)))
#define PWM_MR2           (*((volatile unsigned int *) (0xe0014020)))
#define PWM_MR3           (*((volatile unsigned int *) (0xe0014024)))
#define PWM_MR4           (*((volatile unsigned int *) (0xe0014040)))
#define PWM_MR5           (*((volatile unsigned int *) (0xe0014044)))
#define PWM_MR6           (*((volatile unsigned int *) (0xe0014048)))
#define PWM_EMR           (*((volatile unsigned int *) (0xe001403c)))
#define PWM_PCR           (*((volatile unsigned int *) (0xe001404c)))
#define PWM_LER           (*((volatile unsigned int *) (0xe0014050)))

#define PWM_IR_MR0        ((unsigned int) 0x00000001)
#define PWM_IR_MR1        ((unsigned int) 0x00000002)
#define PWM_IR_MR2        ((unsigned int) 0x00000004)
#define PWM_IR_MR3        ((unsigned int) 0x00000008)
#define PWM_IR_MR4        ((unsigned int) 0x00000100)
#define PWM_IR_MR5        ((unsigned int) 0x00000200)
#define PWM_IR_MR6        ((unsigned int) 0x00000400)
#define PWM_IR_MASK       ((unsigned int) 0x0000070f)

#define PWM_TCR_CE        ((unsigned int) 0x00000001)
#define PWM_TCR_CR        ((unsigned int) 0x00000002)
#define PWM_TCR_PWME      ((unsigned int) 0x00000008)
#define PWM_TCR_MASK      ((unsigned int) 0x0000000b)

#define PWM_MCR_MR0I      ((unsigned int) 0x00000001)
#define PWM_MCR_MR0R      ((unsigned int) 0x00000002)
#define PWM_MCR_MR0S      ((unsigned int) 0x00000004)
#define PWM_MCR_MR1I      ((unsigned int) 0x00000008)
#define PWM_MCR_MR1R      ((unsigned int) 0x00000010)
#define PWM_MCR_MR1S      ((unsigned int) 0x00000020)
#define PWM_MCR_MR2I      ((unsigned int) 0x00000040)
#define PWM_MCR_MR2R      ((unsigned int) 0x00000080)
#define PWM_MCR_MR2S      ((unsigned int) 0x00000100)
#define PWM_MCR_MR3I      ((unsigned int) 0x00000200)
#define PWM_MCR_MR3R      ((unsigned int) 0x00000400)
#define PWM_MCR_MR3S      ((unsigned int) 0x00000800)
#define PWM_MCR_MR4I      ((unsigned int) 0x00001000)
#define PWM_MCR_MR4R      ((unsigned int) 0x00002000)
#define PWM_MCR_MR4S      ((unsigned int) 0x00004000)
#define PWM_MCR_MR5I      ((unsigned int) 0x00008000)
#define PWM_MCR_MR5R      ((unsigned int) 0x00010000)
#define PWM_MCR_MR5S      ((unsigned int) 0x00020000)
#define PWM_MCR_MR6I      ((unsigned int) 0x00040000)
#define PWM_MCR_MR6R      ((unsigned int) 0x00080000)
#define PWM_MCR_MR6S      ((unsigned int) 0x00100000)
#define PWM_MCR_MASK      ((unsigned int) 0x001fffff)

#define PWM_PCR_SEL2      ((unsigned int) 0x00000004)
#define PWM_PCR_SEL3      ((unsigned int) 0x00000008)
#define PWM_PCR_SEL4      ((unsigned int) 0x00000010)
#define PWM_PCR_SEL5      ((unsigned int) 0x00000020)
#define PWM_PCR_SEL6      ((unsigned int) 0x00000040)
#define PWM_PCR_SEL_MASK  ((unsigned int) 0x0000007c)
#define PWM_PCR_ENA1      ((unsigned int) 0x00000200)
#define PWM_PCR_ENA2      ((unsigned int) 0x00000400)
#define PWM_PCR_ENA3      ((unsigned int) 0x00000800)
#define PWM_PCR_ENA4      ((unsigned int) 0x00001000)
#define PWM_PCR_ENA5      ((unsigned int) 0x00002000)
#define PWM_PCR_ENA6      ((unsigned int) 0x00004000)
#define PWM_PCR_ENA_MASK  ((unsigned int) 0x00007d00)
#define PWM_PCR_MASK      ((unsigned int) 0x00007d00)

#define PWM_LER_M0L       ((unsigned int) 0x00000001)
#define PWM_LER_M1L       ((unsigned int) 0x00000002)
#define PWM_LER_M2L       ((unsigned int) 0x00000004)
#define PWM_LER_M3L       ((unsigned int) 0x00000008)
#define PWM_LER_M4L       ((unsigned int) 0x00000010)
#define PWM_LER_M5L       ((unsigned int) 0x00000020)
#define PWM_LER_M6L       ((unsigned int) 0x00000040)
#define PWM_LER_MASK      ((unsigned int) 0x0000007f)

#define PWM_1 (1<<0)
#define PWM_2 (1<<1)
#define PWM_3 (1<<2)
#define PWM_4 (1<<3)
#define PWM_5 (1<<4)
#define PWM_6 (1<<5)


static int pwmPulseWidth = 20000;
static int pwmPercentage[6] = {50,50,50,50,50,50};


int PWM_us_precalc_factor;
int PWM_pc_precalc_factor;



int PWM_dutycycle (int mask, int percentage)
{

	int val = (PWM_pc_precalc_factor * percentage)/100;

  if ((percentage < 0) || (percentage > 100))
    return -1;


  if (mask&PWM_1)
  {
	pwmPercentage[0] = percentage;
	PWM_MR1 = val;
	PWM_LER = PWM_LER_M1L;
  }

  if (mask&PWM_2)
  {
	pwmPercentage[1] = percentage;
	PWM_MR2 = val;
	PWM_LER = PWM_LER_M2L;
  }


  if (mask&PWM_3)
  {
	pwmPercentage[2] = percentage;
	PWM_MR3 = val;
	PWM_LER = PWM_LER_M3L;
  }

  if (mask&PWM_4)
  {
    pwmPercentage[3] = percentage;
	PWM_MR4 = val;
	PWM_LER = PWM_LER_M4L;
  }

  if (mask&PWM_5)
  {
    pwmPercentage[4] = percentage;
	PWM_MR5 = val;
	PWM_LER = PWM_LER_M5L;
  }


  if (mask&PWM_6)
  {
    pwmPercentage[5] = percentage;
	PWM_MR6 = val;
	PWM_LER = PWM_LER_M6L;
  }

  return 1;
}



int PWM_length(int mask, unsigned int count)
{
  int latch_enable = 0;

  if (count > PWM_MR0)
    return -1;

  if (mask&PWM_1)
  {
	PWM_MR1 = count;
	latch_enable |= PWM_LER_M1L;
  }

  if (mask&PWM_2)
  {
	PWM_MR2 = count;
	latch_enable |= PWM_LER_M2L;
  }

  if (mask&PWM_3)
  {
	PWM_MR3 = count;
	latch_enable |= PWM_LER_M3L;
  }

  if (mask&PWM_4)
  {
	PWM_MR4 = count;
	latch_enable |= PWM_LER_M4L;
  }

  if (mask&PWM_5)
  {
	PWM_MR5 = count;
	latch_enable |= PWM_LER_M5L;
  }

  if (mask&PWM_6)
  {
	PWM_MR6 = count;
	latch_enable |= PWM_LER_M6L;
  }

  PWM_LER |= latch_enable;

  return 1;
}


//
//
//



int PWM_pulsewidth_us (int mask, int us)
{
  unsigned int count =  PWM_us_precalc_factor * us;
  PWM_length(mask,count);
  return count;
}


//
//
//

int PWM_frequency (int frequency)
{
  if ((frequency < 1) || (frequency >= VPB_get_speed() ))
    return-1;

  PWM_MR0 = (VPB_get_speed() / (pwmPulseWidth = frequency));
  PWM_pc_precalc_factor = VPB_get_speed() / pwmPulseWidth;
  PWM_MR1 = 0;
  PWM_MR2 = 0;
  PWM_MR3 = 0;
  PWM_MR4 = 0;
  PWM_MR5 = 0;
  PWM_MR6 = 0;
  PWM_LER = PWM_LER_M0L|PWM_LER_M1L|PWM_LER_M2L|PWM_LER_M3L|PWM_LER_M4L|PWM_LER_M5L|PWM_LER_M6L;

  return 1;
}


void PWM_init (int mask)
{
  PCONP |= 1<<5; // Enable PWM power

  if (mask&PWM_1)
	PINSEL0 = (PINSEL0 & ~(3<<0)) | (2<<0); // PWM1 on P0.0 (shared with TXD0)

  if (mask&PWM_2)
	PINSEL0 = (PINSEL0 & ~(3<<14)) | (2<<14); // PWM2 on P0.7 (shared with SSEL0)

  if (mask&PWM_3)
	PINSEL0 = (PINSEL0 & ~(3<<2)) | (2<<2); // PWM3 on P0.1 (shared with RXD0)

  if (mask&PWM_4)
	PINSEL0 = (PINSEL0 & ~(3<<16)) | (2<<16); // PWM4 on P0.8 (shared with TXD1)

  if (mask&PWM_5)
	PINSEL1 = (PINSEL1 & ~(3<<10)) | (1<<10); // PWM5 on P0.21

  if (mask&PWM_6)
	PINSEL0 = (PINSEL0 & ~(3<<18)) | (2<<18); // PWM6 on P0.9 (shared with RXD1)


  PWM_us_precalc_factor = VPB_get_speed() / 1000000;
  PWM_pc_precalc_factor = VPB_get_speed() / pwmPulseWidth;

  PWM_TCR  = PWM_TCR_CR;
  PWM_PR   = 0;
  PWM_MR0  = (VPB_get_speed() / pwmPulseWidth);
  PWM_MCR |= PWM_MCR_MR0R;

 if (mask&PWM_1) PWM_PCR |= PWM_PCR_ENA1;
 if (mask&PWM_2) PWM_PCR |= PWM_PCR_ENA2;
 if (mask&PWM_3) PWM_PCR |= PWM_PCR_ENA3;
 if (mask&PWM_4) PWM_PCR |= PWM_PCR_ENA4;
 if (mask&PWM_5) PWM_PCR |= PWM_PCR_ENA5;
 if (mask&PWM_6) PWM_PCR |= PWM_PCR_ENA6;


  PWM_TCR  = (PWM_TCR_CE | PWM_TCR_PWME);

  PWM_pulsewidth_us (mask,1800);
}


