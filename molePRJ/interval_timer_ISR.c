#include "address_map_nios2.h"
#include "globals.h"					// defines global values

extern volatile int tick;
/*******************************************************************************
 * Interval timer interrupt service routine
 *                                                                          
 * Shifts a PATTERN being displayed on the HEX displays. The shift direction 
 * is determined by the external variable key_pressed.
 ******************************************************************************/
void interval_timer_ISR1( )
{
	volatile int * interval_timer_ptr = (int *) TIMER_BASE;
	//volatile int * HEX3_HEX0_ptr = (int *) HEX3_HEX0_BASE;

	*(interval_timer_ptr) = 0; 			// clear the interrupt
	tick++;
/*
	*(HEX3_HEX0_ptr) = pattern;			// display pattern on HEX3 ... HEX0

	if (shift_enable == DISABLE)		// check if shifting is disabled
		return;
 */
	/* rotate the pattern shown on the HEX displays */
/*	if (shift_dir == LEFT)				// rotate left
		if (pattern & 0x80000000)
			pattern = (pattern << 1) | 1;	
		else
			pattern = pattern << 1;			
	else 								// rotate right
		if (pattern & 0x00000001)			
			pattern = (pattern >> 1) | 0x80000000;
		else
			pattern = (pattern >> 1) & 0x7FFFFFFF;
*/
	return;
}
