#
# Copyright (c) 2009  http:#www.espardino.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#


/* Configuration */

# Stack config

        .equ    Top_Stack,      (0x40000000 + RAM_LENGTH - 0x10)
        .equ    UND_Stack_Size, 0x00000004
        .equ    SVC_Stack_Size, 0x00000020
        .equ    ABT_Stack_Size, 0x00000400
        .equ    FIQ_Stack_Size, 0x00000020
        .equ    IRQ_Stack_Size, 0x00000400
        .equ    SYS_Stack_Size, 0x00000800

# PLL config
        .equ    PLLCFG_Val,     0x00000024   /* mult=5 , div=2 */
# VBPDiv Config
		.equ	VPBDIV_Val,		0x00000002   /* VPB = Main clock */

# MAM config
        .equ    MAMCR_Val,      0x00000002 /* 0- disabled, 1-partially enabled, 2-fully enabled */
        .equ    MAMTIM_Val,     0x00000004



/* Definitions */

        .equ    Mode_USR,       0x10
        .equ    Mode_FIQ,       0x11
        .equ    Mode_IRQ,       0x12
        .equ    Mode_SVC,       0x13
        .equ    Mode_ABT,       0x17
        .equ    Mode_UND,       0x1B
        .equ    Mode_SYS,       0x1F

        .equ    I_Bit,          0x80
        .equ    F_Bit,          0x40


		.equ    VPBDIV,			0xE01FC100

        .equ    PLL_BASE,       0xE01FC080
        .equ    PLLCON_OFS,     0x00
        .equ    PLLCFG_OFS,     0x04
        .equ    PLLSTAT_OFS,    0x08
        .equ    PLLFEED_OFS,    0x0C
        .equ    PLLCON_PLLE,    1
        .equ    PLLCON_PLLC,    2
        .equ    PLLCFG_MSEL,    (0x1F<<0)
        .equ    PLLCFG_PSEL,    (0x03<<5)
        .equ    PLLSTAT_PLOCK,  (1<<10)




        .equ    MAM_BASE,       0xE01FC000  /* MAM Base Address */
        .equ    MAMCR_OFS,      0x00        /* MAM Control Offset*/
        .equ    MAMTIM_OFS,     0x04        /* MAM Timing Offset */



# Startup code

        .text
        .arm


        .global _boot
        .func   _boot
_boot:


Vectors:        LDR     PC, Reset_Addr
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP                            /* Reserved Vector */
                ldr     PC, [PC,#-0xFF0]	   /* VIC Vector jump */
                LDR     PC, FIQ_Addr

Reset_Addr:     .word   Reset_Handler
Undef_Addr:     .word   Undef_Handler
SWI_Addr:       .word   SWI_Handler
// SWI_Addr:  		 .word vPortYieldProcessor       /* FreeRtos: SWI	*/
PAbt_Addr:      .word   PAbt_Handler
DAbt_Addr:      .word   DAbt_Handler
                .word   0                      /* Reserved Address */
IRQ_Addr:       .word   IRQ_Handler
FIQ_Addr:       .word   FIQ_Handler

Undef_Handler:  B       Undef_Handler
SWI_Handler:    B       SWI_Handler
PAbt_Handler:   B       PAbt_Handler
DAbt_Handler:   B       DAbt_Handler
IRQ_Handler:    B       IRQ_Handler
FIQ_Handler:    B       FIQ_Handler

Reset_Handler:

# Setup the PLL

                LDR     R0, =PLL_BASE
                MOV     R1, #0xAA
                MOV     R2, #0x55

                MOV     R3, #PLLCFG_Val
                STR     R3, [R0, #PLLCFG_OFS]
                MOV     R3, #PLLCON_PLLE
                STR     R3, [R0, #PLLCON_OFS]
                STR     R1, [R0, #PLLFEED_OFS]
                STR     R2, [R0, #PLLFEED_OFS]

# Wait for pll locked
PLL_Loop:       LDR     R3, [R0, #PLLSTAT_OFS]
                ANDS    R3, R3, #PLLSTAT_PLOCK
                BEQ     PLL_Loop

# Enable PLL output as clock
                MOV     R3, #(PLLCON_PLLE | PLLCON_PLLC)
                STR     R3, [R0, #PLLCON_OFS]
                STR     R1, [R0, #PLLFEED_OFS]
                STR     R2, [R0, #PLLFEED_OFS]


				MOV R1, #VPBDIV_Val
				LDR R0,=VPBDIV
				STR R1,[R0,#0]


# Setup the memory acelerator module

                LDR     R0, =MAM_BASE
                MOV     R1, #MAMTIM_Val
                STR     R1, [R0, #MAMTIM_OFS]
                MOV     R1, #MAMCR_Val
                STR     R1, [R0, #MAMCR_OFS]


# Setup Stack for each mode

                LDR     R0, =Top_Stack

#  Enter Undefined Instruction Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

#  Enter Abort Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

#  Enter FIQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

#  Enter IRQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

#  Enter Supervisor Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size


#  Enter User Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_SYS
                MOV     SP, R0

#  Setup a default Stack Limit. useful for "-mapcs-stack-check" gcc option
                SUB     SL, SP, #SYS_Stack_Size

/* We want to start in supervisor mode.  Operation will switch to system
	mode when the first task starts. */
				msr   CPSR_c, #Mode_SVC|I_Bit|F_Bit



# Relocate .data section (Copy from ROM to RAM)
                LDR     R1, =_etext
                LDR     R2, =_data
                LDR     R3, =_edata
LoopRel:        CMP     R2, R3
                LDRLO   R0, [R1], #4
                STRLO   R0, [R2], #4
                BLO     LoopRel


# Clear .bss section (Zero init)
                MOV     R0, #0
                LDR     R1, =__bss_start__
                LDR     R2, =__bss_end__
LoopZI:         CMP     R1, R2
                STRLO   R0, [R1], #4
                BLO     LoopZI

# Call C++ Constructors

		LDR 	r0, =__ctors_start__
		LDR 	r1, =__ctors_end__
ctor_loop:
		CMP 	r0, r1
		BEQ 	ctor_end
		LDR 	r2, [r0], #4
		STMFD 	sp!, {r0-r1}
		MOV 	lr, pc
		MOV 	pc, r2
		LDMFD 	sp!, {r0-r1}
		B 		ctor_loop
ctor_end:

# Enter the C code

                LDR     R0,=main
                TST     R0,#1             // Bit-0 set: main is Thumb
                LDREQ   LR,=__exit_ARM    // ARM Mode
                LDRNE   LR,=__exit_THUMB  // Thumb Mode
                BX      R0

.size   _boot, . - _boot
.endfunc

.arm
.global __exit_ARM
.func __exit_ARM
__exit_ARM:
                B       __exit_ARM
.size   __exit_ARM, . - __exit_ARM
.endfunc

.thumb
.global __exit_THUMB
.func __exit_THUMB
__exit_THUMB:
                B       __exit_THUMB
.size   __exit_THUMB, . - __exit_THUMB
.endfunc

        .end
