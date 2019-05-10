	.include "address_map_nios2.s"
/*******************************************************************************
 * Updates the value displayed on the hex display. The value is taken from the 
 * buffer.
 *
 ******************************************************************************/
	.global _CNT
	.global UPDATE_HEX_DISPLAY
UPDATE_HEX_DISPLAY:
	subi	sp, sp, 36				# reserve space on the stack 
	/* save registers */
	stw		ra, 0(sp)
	stw		fp, 4(sp)
	stw 	r15, 8(sp)
	stw 	r16, 12(sp)
	stw 	r17, 16(sp)
	stw 	r18, 20(sp)
	stw 	r19, 24(sp)
	stw 	r20, 28(sp)
	stw 	r21, 32(sp)
	addi	fp, sp, 36

	/* load hex value to display */
	movia	r15	,_CNT#movia	r15, DISPLAY_BUFFER
	ldwio		r16, 0(r15)#4(r15)				# value to display is in second full-word of buffer 
	#movia r16,0x1234
/*******************************************************************************
 * Loop to fill the two-word buffer that drives the parallel port on the computer 
 * system connected to the HEX5 to HEX0 displays. The loop produces for each 4-bit
 * character in r16 a corresponding 8-bit code for the segments of the displays
 ******************************************************************************/
	movia	r17, 4
	movia	r15, HEX3_HEX0	
	movia	r19, SEVEN_SEG_DECODE_TABLE
SEVEN_SEG_DECODER:
	mov		r18, r16
	andi	r18, r18, 0x000F
	add		r20, r19, r18			# index into decode table based on character 
	add		r21, zero, zero
	ldb		r21, 0(r20)				# r21 <- 7-seg character code 
	stb		r21, 0(r15)				# store 7-seg code into buffer 

	srli	r16, r16, 4
	addi	r15, r15, 1
	subi	r17, r17, 1
	bge		r17, zero, SEVEN_SEG_DECODER

	/* write parallel port buffer words */
	movia	r15, HEX3_HEX0
	ldw		r16, 0(r15)
	movia	r17, HEX3_HEX0_BASE
	stwio	r16, 0(r17)
	ldw		r16, 4(r15)
	movia	r17, HEX5_HEX4_BASE
	stwio	r16, 0(r17)

	/* restore registers */
	ldw		ra, 0(sp)
	ldw		fp, 4(sp)
	ldw 	r15, 8(sp)
	ldw 	r16, 12(sp)
	ldw 	r17, 16(sp)
	ldw 	r18, 20(sp)
	ldw 	r19, 24(sp)
	ldw 	r20, 28(sp)
	ldw 	r21, 32(sp)
	addi	sp, sp, 36				# release the reserved stack space 

	ret

/******************************************************************************/
HEX3_HEX0:
	.word 0
HEX5_HEX4:
	.word 0
_CNT:
	.word	1
HEX_bits:
	.word 0x0000000F
SEVEN_SEG_DECODE_TABLE:
seven_seg_decode_table:
	.byte 0x3F
	.byte 0x06
	.byte 0x5B
	.byte 0x4F
	.byte 0x66
	.byte 0x6D
	.byte 0x7D
	.byte 0x07
	.byte 0x7F
	.byte 0x67
	.byte 0x77
	.byte 0x7C
	.byte 0x39
	.byte 0x5E
	.byte 0x79
	.byte 0x71
	.end