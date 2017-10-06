#include "include/includes.h"


//-----------------------------------------------------------------------------------
//	void halWait(UINT16 timeout)
//
//	DESCRIPTION:
//		Runs an idle loop for [timeout] microseconds.
//
//  ARGUMENTS:
//      UINT16 timeout
//          The timeout in microseconds
//-----------------------------------------------------------------------------------
void halWaitsensor(UINT16 timeout)
{
    // This sequence uses exactly 8 clock cycles for each round
    do {
    	P5OUT |= 0x70;
    	
        _NOP();
    } while (--timeout);
    

} // halWait
