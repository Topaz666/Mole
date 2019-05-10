	.include "address_map_nios2.s"

	.extern tick
	.global	interval_timer_ISR
interval_timer_ISR:
	subi	sp,  sp, 20			# reserve space on the stack
	stw		r3, 0(sp)
	stw		r2, 4(sp)

	movia	r3, TIMER_BASE	# Address of switches
	movia	r2,0
	sthio	r2,0(r3)
	movia	r3,tick
	ldwio	r2,0(r3)
	addi	r2,r2,1
	stwio	r2,0(r3)

	ldw		r2, 4(sp)
	ldw		r3,  0(sp)
	addi	sp,  sp, 20			# release the reserved space on the stack 
	ret
	.end
