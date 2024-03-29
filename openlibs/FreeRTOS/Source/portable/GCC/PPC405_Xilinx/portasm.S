/*
	FreeRTOS.org V5.3.0 - Copyright (C) 2003-2009 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.
	**NOTE** The exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS.org without being obliged to provide
	the source code for any proprietary components.  Alternative commercial
	license and support terms are also available upon request.  See the 
	licensing section of http://www.FreeRTOS.org for full details.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.


	***************************************************************************
	*                                                                         *
	* Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

#include "FreeRTOSConfig.h"

	.extern pxCurrentTCB
	.extern vTaskSwitchContext
	.extern vTaskIncrementTick
	.extern vPortISRHandler

	.global vPortStartFirstTask
	.global vPortYield
	.global vPortTickISR
	.global vPortISRWrapper
	.global vPortSaveFPURegisters
	.global vPortRestoreFPURegisters

.set	BChainField, 0
.set	NextLRField, BChainField + 4
.set	MSRField,    NextLRField + 4
.set	PCField,     MSRField    + 4
.set	LRField,     PCField     + 4
.set	CTRField,    LRField     + 4
.set	XERField,    CTRField    + 4
.set	CRField,     XERField    + 4
.set	USPRG0Field, CRField     + 4
.set	r0Field,     USPRG0Field + 4
.set	r2Field,     r0Field     + 4
.set	r3r31Field,  r2Field     + 4
.set	IFrameSize,  r3r31Field  + ( ( 31 - 3 ) + 1 ) * 4


.macro portSAVE_STACK_POINTER_AND_LR

	/* Get the address of the TCB. */
	xor		R0, R0, R0
	addis	R2, R0, pxCurrentTCB@ha
	lwz		R2,	pxCurrentTCB@l( R2 )

	/* Store the stack pointer into the TCB */
	stw		SP,	0( R2 )

	/* Save the link register */
	stwu	R1, -24( R1 )
	mflr	R0
	stw		R31, 20( R1 )
	stw		R0, 28( R1 )
	mr		R31, r1

.endm

.macro portRESTORE_STACK_POINTER_AND_LR

	/* Restore the link register */
	lwz		R11, 0( R1 )
	lwz		R0, 4( R11 )
	mtlr	R0
	lwz		R31, -4( R11 )
	mr		R1, R11

	/* Get the address of the TCB. */
	xor		R0, R0, R0
	addis   SP, R0, pxCurrentTCB@ha
	lwz		SP,	pxCurrentTCB@l( R1 )

	/* Get the task stack pointer from the TCB. */
	lwz		SP, 0( SP )

.endm


vPortStartFirstTask:

	/* Get the address of the TCB. */
	xor		R0, R0, R0
    addis   SP, R0, pxCurrentTCB@ha
    lwz		SP,	pxCurrentTCB@l( SP )

	/* Get the task stack pointer from the TCB. */
	lwz		SP, 0( SP )
	
	/* Restore MSR register to SRR1. */
	lwz		R0, MSRField(R1)
	mtsrr1	R0
	
	/* Restore current PC location to SRR0. */
	lwz		R0, PCField(R1)
	mtsrr0	R0

	/* Save  USPRG0 register */
	lwz		R0, USPRG0Field(R1)
	mtspr	0x100,R0
	
	/* Restore Condition register */
	lwz		R0, CRField(R1)
	mtcr	R0
	
	/* Restore Fixed Point Exception register */
	lwz		R0, XERField(R1)
	mtxer	R0
	
	/* Restore Counter register */
	lwz		R0, CTRField(R1)
	mtctr	R0
	
	/* Restore Link register */
	lwz		R0, LRField(R1)
	mtlr	R0
	
	/* Restore remaining GPR registers. */
	lmw	R3,r3r31Field(R1)
	
	/* Restore r0 and r2. */
	lwz		R0, r0Field(R1)
	lwz		R2, r2Field(R1)
	
	/* Remove frame from stack */
	addi	R1,R1,IFrameSize

	/* Return into the first task */
	rfi



vPortYield:

	portSAVE_STACK_POINTER_AND_LR
	bl vTaskSwitchContext
	portRESTORE_STACK_POINTER_AND_LR
	blr

vPortTickISR:

	portSAVE_STACK_POINTER_AND_LR
	bl vTaskIncrementTick
	
	#if configUSE_PREEMPTION == 1
		bl vTaskSwitchContext
	#endif

	/* Clear the interrupt */
	lis		R0, 2048
	mttsr	R0

	portRESTORE_STACK_POINTER_AND_LR
	blr

vPortISRWrapper:

	portSAVE_STACK_POINTER_AND_LR
	bl vPortISRHandler
	portRESTORE_STACK_POINTER_AND_LR
	blr

#if configUSE_FPU == 1

vPortSaveFPURegisters:

	/* Enable APU and mark FPU as present. */
	mfmsr	r0
	xor		r30, r30, r30
	oris	r30, r30, 512
	ori		r30, r30, 8192
	or		r0, r0, r30
	mtmsr	r0

	/* Buffer address is in r3.  Save each flop register into an offset from
	this buffer address. */
	stfs	f0, 0(r3)
	stfs	f1, 4(r3)
	stfs	f2, 8(r3)
	stfs	f3, 12(r3)
	stfs	f4, 16(r3)
	stfs	f5, 20(r3)
	stfs	f6, 24(r3)
	stfs	f7, 28(r3)
	stfs	f8, 32(r3)
	stfs	f9, 36(r3)
	stfs	f10, 40(r3)
	stfs	f11, 44(r3)
	stfs	f12, 48(r3)
	stfs	f13, 52(r3)
	stfs	f14, 56(r3)
	stfs	f15, 60(r3)
	stfs	f16, 64(r3)
	stfs	f17, 68(r3)
	stfs	f18, 72(r3)
	stfs	f19, 76(r3)
	stfs	f20, 80(r3)
	stfs	f21, 84(r3)
	stfs	f22, 88(r3)
	stfs	f23, 92(r3)
	stfs	f24, 96(r3)
	stfs	f25, 100(r3)
	stfs	f26, 104(r3)
	stfs	f27, 108(r3)
	stfs	f28, 112(r3)
	stfs	f29, 116(r3)
	stfs	f30, 120(r3)
	stfs	f31, 124(r3)
	
	/* Also save the FPSCR. */
	mffs	f31
	stfs	f31, 128(r3)

	blr

#endif /* configUSE_FPU. */


#if configUSE_FPU == 1

vPortRestoreFPURegisters:

	/* Enable APU and mark FPU as present. */
	mfmsr	r0
	xor		r30, r30, r30
	oris	r30, r30, 512
	ori		r30, r30, 8192
	or		r0, r0, r30
	mtmsr	r0


	/* Buffer address is in r3.  Restore each flop register from an offset
	into this buffer. 
	
	First the FPSCR. */
	lfs		f31, 128(r3)
	mtfsf	f31, 7

	lfs		f0, 0(r3)
	lfs		f1, 4(r3)
	lfs		f2, 8(r3)
	lfs		f3, 12(r3)
	lfs		f4, 16(r3)
	lfs		f5, 20(r3)
	lfs		f6, 24(r3)
	lfs		f7, 28(r3)
	lfs		f8, 32(r3)
	lfs		f9, 36(r3)
	lfs		f10, 40(r3)
	lfs		f11, 44(r3)
	lfs		f12, 48(r3)
	lfs		f13, 52(r3)
	lfs		f14, 56(r3)
	lfs		f15, 60(r3)
	lfs		f16, 64(r3)
	lfs		f17, 68(r3)
	lfs		f18, 72(r3)
	lfs		f19, 76(r3)
	lfs		f20, 80(r3)
	lfs		f21, 84(r3)
	lfs		f22, 88(r3)
	lfs		f23, 92(r3)
	lfs		f24, 96(r3)
	lfs		f25, 100(r3)
	lfs		f26, 104(r3)
	lfs		f27, 108(r3)
	lfs		f28, 112(r3)
	lfs		f29, 116(r3)
	lfs		f30, 120(r3)
	lfs		f31, 124(r3)

	blr

#endif /* configUSE_FPU. */


