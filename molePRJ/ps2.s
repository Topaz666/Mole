	.include "address_map_nios2.s"

	.extern PS2_data
	.extern RVALID
	.global	get_ps2
get_ps2:
	subi	sp,  sp, 20			# reserve space on the stack
	stw		r3, 0(sp)
	stw		r2, 4(sp)
	movia	r3, PS2_BASE	# Address of switches
	ldwio	r2, (r3)
	movia	r3, PS2_data
	stwio	r2,0(r3)
	movia	r3,RVALID
	andi	r2,r2,0x8000
	stwio	r2,0(r3)
	ldw		r2, 4(sp)
	ldw		r3,  0(sp)
	addi	sp,  sp, 20			# release the reserved space on the stack 
	ret
	.end
