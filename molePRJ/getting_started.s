	.include "address_map_nios2.s"

/******************************************************************************
 * This program demonstrates use of parallel ports
 *
 * It performs the following: 
 * 	1. displays the SW switch values on the red LEDs
 * 	2. displays a rotating pattern on the HEX displays
 * 	3. if any KEY is pressed, the SW switches are used as the rotating pattern
 ******************************************************************************/

	.text						# executable code follows
	#.extern cnt
	.global _getkey
_getkey:
	subi	sp,  sp, 20			# reserve space on the stack
	stw		r3, 0(sp)
	movia	r3, KEY_BASE	# Address of switches
	ldwio	r2, (r3)
	ldw		r3,  0(sp)
	addi	sp,  sp, 20			# release the reserved space on the stack 
	ret
	.end
