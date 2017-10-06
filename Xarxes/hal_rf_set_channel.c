/************************************************************************************

  Filename:     hal_rf_set_channel.c

  Description:  The Chipcon Hardware Abstraction Library is a collection of functions,
                macros and constants, which can be used to ease access to the hardware
                on the CC2420 and the target microcontroller.

                This file contains a function that allows you to switch radio channels
                on the CC2420.

************************************************************************************/

/************************************************************************************

   EXAMPLE OF USAGE:

       // Turn off RX...
       DISABLE_GLOBAL_INT();
       FASTSPI_STROBE(CC2420_SRFOFF);
       ENABLE_GLOBAL_INT();

       // ... switch to the next channel in the loop ...
       halRfSetChannel(channel++);
       if (channel == 27) channel = 11;

       // ... and go back into RX
       DISABLE_GLOBAL_INT();
       FASTSPI_STROBE(CC2420_SRXON);
       ENABLE_GLOBAL_INT();

************************************************************************************/
#include "include/includes.h"


//-----------------------------------------------------------------------------------
//	void halRfSetChannel(UINT8 Channel)
//
//	DESCRIPTION:
//		Programs CC2420 for a given IEEE 802.15.4 channel.
//		Note that SRXON, STXON or STXONCCA must be run for the new channel selection to take full effect.
//
//	PARAMETERS:
//		UINT8 channel
//			The channel number (11-26)
//-----------------------------------------------------------------------------------
void halRfSetChannel(UINT8 channel)
{
	UINT16 f;
	
	// Derive frequency programming from the given channel number
	f = (UINT16) (channel - 11); // Subtract the base channel
	f = f + (f << 2);    		 // Multiply with 5, which is the channel spacing
	f = f + 357 + 0x4000;		 // 357 is 2405-2048, 0x4000 is LOCK_THR = 1
	
    // Write it to the CC2420
	DISABLE_GLOBAL_INT();
	FASTSPI_SETREG(CC2420_FSCTRL, f);
	ENABLE_GLOBAL_INT();

} // rfSetChannel



/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
