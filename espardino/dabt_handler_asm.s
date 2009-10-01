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
  .text
        .arm
	

.equ    THUMB_bit,  (1<<5)  


 .global _PABT_Handler_espardino_debug
 .func   _PABT_Handler_espardino_debug

_PABT_Handler_espardino_debug:  
	STMFD SP!, {R0-R12}
	MOV R8,#2					// we are storing a DABT
	B _cont_espardino_debug
.endfunc

.global _UND_Handler_espardino_debug
.func _UND_Handler_espardino_debug
_UND_Handler_espardino_debug:  
	STMFD SP!, {R0-R12}
	MOV R8,#3					// we are storing a DABT
	B _cont_espardino_debug
.endfunc

 .global _DABT_Handler_espardino_debug
 .func   _DABT_Handler_espardino_debug

_DABT_Handler_espardino_debug:  
	STMFD SP!, {R0-R12}
	MOV R8,#1					// we are storing a DABT

_cont_espardino_debug:
	
	STR R8,[SP,#-4]!			// store the type of exception
	
	MRS R5,CPSR    			 	// current mode to r5
	BIC R5,R5,#I_Bit|F_Bit 		// but IRQ/FIQ enabled
	MRS R6,SPSR     			// saved status register to r6
	
	
	CMP R8,#1
	BNE _not_DABT_pc
	
	SUB R0,LR,#8				// calculate pc value of dabt instruction
	B _pc_offset_calculated
	
_not_DABT_pc:
	CMP R8,#2
	BNE _not_PABT_pc
	
	SUB R0,LR,#4      // Prefetch abort R0 = LR(PC) -4
	B _pc_offset_calculated
	
_not_PABT_pc:
				// calculate pc value for undefined instruction and Prefetch abort
	
	TST R6,#THUMB_bit			
	SUBEQ R0,LR,#4		// ARM instructions  
	SUBNE R0,LR,#2      // Thumb instructions

_pc_offset_calculated:	
	
					// R6 is SPSR, R5 is unmasked CPSR
	MOV R2,R6    
	TST R6,#0xF      //if mode USR
	ORREQ R6,R6,#0xF // mode = SYS
	BIC R7,R6,#THUMB_bit // force ARM mode 
	
	MSR CPSR_c,r7    // enter into "excepted mode, ARM forced, not USR->SYS"
	MOV R3,LR	     // to catch LR
	MOV R4,SP        // and SP 
	MSR CPSR_c,r5    // return to dabt handler mode
	
	CMP R8,#2		// Are we on Prefetch abort?
	BEQ _on_PABT
	
	TST R6,#THUMB_bit
	LDRNEH R1,[R0]	// Thumb state: Fetch only 16 bits to r1
	LDREQ R1,[R0]   // ARM state  : Fetch full 32 bits to r1
	
_on_PABT:

	STMFD SP!,{R0-R4} // R0 = ADDR, R1 = INSTR , R2 = CSPR, R3 = LR,  R4 = SP
	
	MOV R0,SP // r0 = pointer to start of dump
	
	
	LDR R1,=dabt_dump
	BX R1
.thumb

__exit_dabt_dump: 
		B __exit_dabt_dump
		
.endfunc

.end
				
