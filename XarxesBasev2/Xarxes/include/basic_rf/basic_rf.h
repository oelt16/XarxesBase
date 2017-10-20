/************************************************************************************

  Filename:     basic_rf.h

  Description:  Basic RF library interface.

    The "Basic RF" library contains simple functions for packet transmission and
    reception with the CC2520 radio chip. The intention of this library is mainly
    to demonstrate how the CC2520 is operated, and not to provide a complete and
    fully-functional packet protocol. The protocol uses 802.15.4 MAC compliant data
    and acknowledgment packets, however it contains only a small subset of  the
    802.15.4 standard:
    - Association, scanning nor beacons are not implemented
    - No defined coordinator/device roles (peer-to-peer, all nodes are equal)
    - Waits for the channel to become ready, but does not check CCA twice
    (802.15.4 CSMA-CA)
    - Does not retransmit packets

    INSTRUCTIONS:
    Startup:
    1. Create a basicRfCfg_t structure, and initialize the members:
    2. Call basicRfInit() to initialize the packet protocol.

    Transmission:
    1. Create a buffer with the payload to send
    2. Call basicRfSendPacket()

    Reception:
    1. Check if a packet is ready to be received by highger layer with
    basicRfPacketIsReady()
    2. Call basicRfReceive() to receive the packet by higher layer

    FRAME FORMATS:
    Data packets (without security):
    [Preambles (4)][SFD (1)][Length (1)][Frame control field (2)]
    [Sequence number (1)][PAN ID (2)][Dest. address (2)][Source address (2)]
    [Payload (Length - 2+1+2+2+2)][Frame check sequence (2)]

    Acknowledgment packets:
    [Preambles (4)][SFD (1)][Length = 5 (1)][Frame control field (2)]
    [Sequence number (1)][Frame check sequence (2)]
***********************************************************************************/
#ifndef BASIC_RF_H
#define BASIC_RF_H

/***********************************************************************************
* PUBLIC CONSTANTS
*/

//-----------------------------------------------------------------------------------
// Constants concerned with the Basic RF packet format

// Packet overhead ((frame control field, sequence number, PAN ID, destination and source) + (footer))
// Note that the length UINT8 itself is not included included in the packet length
#define BASIC_RF_PACKET_OVERHEAD_SIZE   ((2 + 1 + 2 + 2 + 2) + (2))
#define BASIC_RF_MAX_PAYLOAD_SIZE		(127 - BASIC_RF_PACKET_OVERHEAD_SIZE)
#define BASIC_RF_ACK_PACKET_SIZE		5

// The time it takes for the acknowledgment packet to be received after the data packet has been
// transmitted
#define BASIC_RF_ACK_DURATION			(0.5 * 32 * 2 * ((4 + 1) + (1) + (2 + 1) + (2)))
#define BASIC_RF_SYMBOL_DURATION	    (32 * 0.5)

// The length UINT8
#define BASIC_RF_LENGTH_MASK            0x7F

// Frame control field
#define BASIC_RF_FCF_NOACK              0x8841
#define BASIC_RF_FCF_ACK                0x8861
#define BASIC_RF_FCF_ACK_BM             0x0020
#define BASIC_RF_FCF_BM                 (~BASIC_RF_FCF_ACK_BM)
#define BASIC_RF_ACK_FCF		        0x0002

// Footer
#define BASIC_RF_CRC_OK_BM              0x80
//-----------------------------------------------------------------------------------


/***********************************************************************************
* PUBLIC DATA TYPES
*/

//-----------------------------------------------------------------------------------
// The data structure which is used to transmit packets
typedef struct {
    UINT16 destPanId;
	UINT16 destAddr;
	INT8 length;
    UINT8 *pPayload;
	BOOL ackRequest;
} BASIC_RF_TX_INFO;

//-----------------------------------------------------------------------------------
// The receive struct:
typedef struct {
    UINT8 seqNumber;
	UINT16 srcAddr;
	UINT16 srcPanId;
	INT8 length;
    UINT8 *pPayload;
	BOOL ackRequest;
	INT8 rssi;
} BASIC_RF_RX_INFO;


//-----------------------------------------------------------------------------------
// The RF settings structure:
typedef struct {
    BASIC_RF_RX_INFO *pRxInfo;
    UINT8 txSeqNumber;
    volatile BOOL ackReceived;
    UINT16 panId;
    UINT16 myAddr;
    BOOL receiveOn;
} BASIC_RF_SETTINGS;



/***********************************************************************************
* PUBLIC DATA
*/

extern volatile BASIC_RF_SETTINGS rfSettings;


/***********************************************************************************
* PUBLIC FUNCTIONS
*/

//-----------------------------------------------------------------------------------
//  UINT8 basicRfSendPacket(BASIC_RF_TX_INFO *pRTI)
//
//  DESCRIPTION:
//		Transmits a packet using the IEEE 802.15.4 MAC data packet format with short addresses. CCA is
//		measured only once before backet transmission (not compliant with 802.15.4 CSMA-CA).
//		The function returns:
//			- When pRTI->ackRequest is FALSE: After the transmission has begun (SFD gone high)
//			- When pRTI->ackRequest is TRUE: After the acknowledgment has been received/declared missing.
//		The acknowledgment is received through the FIFOP interrupt.
//
//  ARGUMENTS:
//      BASIC_RF_TX_INFO *pRTI
//          The transmission structure, which contains all relevant info about the packet.
//
//  RETURN VALUE:
//		BOOL
//			Successful transmission (acknowledgment received)
//-----------------------------------------------------------------------------------
BOOL basicRfSendPacket(BASIC_RF_TX_INFO *pRTI);



//-----------------------------------------------------------------------------------
//  void halRfReceiveOn(void)
//
//  DESCRIPTION:
//      Enables the CC2420 receiver and the FIFOP interrupt. When a packet is received through this
//      interrupt, it will call halRfReceivePacket(...), which must be defined by the application
//-----------------------------------------------------------------------------------
void basicRfReceiveOn(void);


//-----------------------------------------------------------------------------------
//  void halRfReceiveOff(void)
//
//  DESCRIPTION:
//      Disables the CC2420 receiver and the FIFOP interrupt.
//-----------------------------------------------------------------------------------
void basicRfReceiveOff(void);


//-----------------------------------------------------------------------------------
//  SIGNAL(SIG_INTERRUPT0) - CC2420 FIFOP interrupt service routine
//
//  DESCRIPTION:
//		When a packet has been completely received, this ISR will extract the data from the RX FIFO, put
//		it into the active BASIC_RF_RX_INFO structure, and call basicRfReceivePacket() (defined by the
//		application). FIFO overflow and illegally formatted packets is handled by this routine.
//
//      Note: Packets are acknowledged automatically by CC2420 through the auto-acknowledgment feature.
//-----------------------------------------------------------------------------------
// SIGNAL(SIG_INTERRUPT0)


//-----------------------------------------------------------------------------------
//  BASIC_RF_RX_INFO* basicRfReceivePacket(BASIC_RF_RX_INFO *pRRI)
//
//  DESCRIPTION:
//      This function is a part of the basic RF library, but must be declared by the application. Once
//		the application has turned on the receiver, using basicRfReceiveOn(), all incoming packets will
//		be received by the FIFOP interrupt service routine. When finished, the ISR will call the
//		basicRfReceivePacket() function. Please note that this function must return quickly, since the
//		next received packet will overwrite the active BASIC_RF_RX_INFO structure (pointed to by pRRI).
//
//  ARGUMENTS:
//		BASIC_RF_RX_INFO *pRRI
//	      	The reception structure, which contains all relevant info about the received packet.
//
//  RETURN VALUE:
//     BASIC_RF_RX_INFO*
//			The pointer to the next BASIC_RF_RX_INFO structure to be used by the FIFOP ISR. If there is
//			only one buffer, then return pRRI.
//-----------------------------------------------------------------------------------
BASIC_RF_RX_INFO* basicRfReceivePacket(BASIC_RF_RX_INFO *pRRI);


//-------------------------------------------------------------------------------------------------------
//  void basicRfInit(BASIC_RF_RX_INFO *pRRI, UINT8 channel, UINT16 panId, UINT16 myAddr)
//
//  DESCRIPTION:
//      Initializes CC2420 for radio communication via the basic RF library functions. Turns on the
//		voltage regulator, resets the CC2420, turns on the crystal oscillator, writes all necessary
//		registers and protocol addresses (for automatic address recognition). Note that the crystal
//		oscillator will remain on (forever).
//
//  ARGUMENTS:
//      BASIC_RF_RX_INFO *pRRI
//          A pointer the BASIC_RF_RX_INFO data structure to be used during the first packet reception.
//			The structure can be switched upon packet reception.
//      UINT8 channel
//          The RF channel to be used (11 = 2405 MHz to 26 = 2480 MHz)
//      UINT16 panId
//          The personal area network identification number
//      UINT16 myAddr
//          The 16-bit short address which is used by this node. Must together with the PAN ID form a
//			unique 32-bit identifier to avoid addressing conflicts. Normally, in a 802.15.4 network, the
//			short address will be given to associated nodes by the PAN coordinator.
//-------------------------------------------------------------------------------------------------------
void basicRfInit(BASIC_RF_RX_INFO *pRRI, UINT8 channel, UINT16 panId, UINT16 myAddr);


#endif

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
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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
