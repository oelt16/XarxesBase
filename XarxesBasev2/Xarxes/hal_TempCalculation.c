/*****************************************************************************
 *                                                                           *
 *        **********                                                         *
 *       ************                                                        *
 *      ***        ***                                                       *
 *      ***   +++   ***                                                      *
 *      ***   + +   ***                                                      *
 *      ***   +                            CHIPCON CC2420                    *
 *      ***   + +   ***             	CC2420 HAL LIBRARY FILE              *
 *      ***   +++   ***                                                      *
 *      ***       ***                                                        *
 *       ***********                                                         *
 *        *********                                                          *
 *                                                                           *
 *****************************************************************************
 *       																     *
 * A function which is a part of the Hardware Abstraction Layer for CC2420DB *
 * with an ATmega128 microcontroller.                                        *             *
 *                                                                           *
 *****************************************************************************
 * Device:      CC2420                                                       *
 * File:		hal_TempCalculation.h					                     *            *
 * Author:		MBR						                                     *
 * Target:		ATmega128						                             *
 * Created:		2004-02-11					                                 *
 *				    			                                             *
 *****************************************************************************
 * Revision history:                                                         *
 *                                                                           *
 * 
 *																			 *
 *																			 *
 *                                                                           *
 ****************************************************************************/
#include "include/includes.h"

// Global SPI status byte from CC2420
BYTE spiStatusByte;
//----------------------------------------------------------------------------
//  INT16 hal_TempCalculation(INT16 hadc_value)
//
//  Description:
//      This function calculates the temperature base on the
//      ADC value found by the internal Atmega8 ADC in degrees
//		Celsius.
//
//  Arguments:
//      INT16 hadc_value
//          The ADC value found for the ADC conversion preformed
//  Return value:
//      Temperature in degrees Celsius
//----------------------------------------------------------------------------
INT16 hal_TempCalculation(INT16 hadc_value) {
	float temp;
	UINT16 htemperature;
	temp = ((((float)hadc_value * V_REF) / 1024.0) - 0.6 ) / 0.01;      		
	
	if(((float)temp - (INT16)temp) < 0.5) {
		htemperature = (INT16)temp;
	} else{
		htemperature = (INT16)temp + 1;
	}
	return(htemperature);
} // hal_TempCalculation
