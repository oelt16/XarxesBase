#include "include/includes.h"
#include "include/blink.h"
#include "include/basic_rf/basic_rf.h"
#include "include/hal/msp430/hal.h"
#include "include/hal/msp430/hal_msp430FET.h"
#include "include/hal/hal_cc2420.h"
#include <time.h>
#include <stdio.h>


BOOL bucleInicial = TRUE;
// The RF settings structure is declared here, since we'll always need halRfInit()
volatile BASIC_RF_SETTINGS rfSettings;

//-----------------------------------------------------------------------------------
// PRIVATE CONSTANTS

#define PAYLOAD_SIZE	10
#define RF_CHANNEL		15
#define TX_PERIOD       50  // Packet sent each n'th cycle

#define PANID			0x2420
//Request variables
#define START_REQUEST 0x10
#define ACK 0x55
#define SLEEP_REQUEST 0x20
#define DATA_REQUEST 0x40

#define START_RESPONSE 0x11
#define SLEEP_RESPONSE 0x21
#define WAKE_UP 0x30
#define DATA 0x41

//receptor tiene 30arriba 50 ABAJO

#define MYADDR			0x1714
#define DESADDR			0x1416
INT8 x;
INT8 estado;
BOOL wait;
INT8 foo;
int i;
INT8 recibidos;
UINT8 request;
#ifdef __ICC430__
#define FILL_UINT8    0xFF
#else
#define FILL_UINT8    0xEE
#endif

//-----------------------------------------------------------------------------------
// PRIVATE DATA

static UINT8 ledPeriod= 0x80;    // LED blinking frequency
static UINT16 nRecv =  0;       // Counting received packets

// Basic RF transmission and reception structures
static BASIC_RF_RX_INFO rfRxInfo;
static BASIC_RF_TX_INFO rfTxInfo;
static UINT8 pTxBuffer[BASIC_RF_MAX_PAYLOAD_SIZE];
static UINT8 pRxBuffer[BASIC_RF_MAX_PAYLOAD_SIZE];
void wait_response();
void enviar_trama(UINT8 data);
void rfshutdown();
//-----------------------------------------------------------------------------------
// PRIVATE FUNCTION PROTOTYPES

//static void ledFlash(UINT16 duration, UINT16 period);


/*********************************************************************
 * Metode main (void)
 * description:
 * 	Startup routine and main loop
 ********************************************************************/
int main(void) {

    unsigned int iclk = 0;
    UINT32 iLoopCount;

    // Initalize ports for communication with CC2420 and other peripheral units
    PORT_INIT();
    SPI_INIT();
    InitP2_7();
    estado = 0;
    recibidos = 0;
    // Wait for the user to select node address, and initialize for basic RF operation
	halWait(1000);

	basicRfInit(&rfRxInfo, RF_CHANNEL, PANID, MYADDR);
	rfTxInfo.destAddr = DESADDR;
	rfTxInfo.destPanId = PANID;


    // Initalize common protocol parameters
    rfTxInfo.length = PAYLOAD_SIZE;
    rfTxInfo.ackRequest = FALSE;
    rfTxInfo.pPayload = pTxBuffer;
    rfRxInfo.pPayload = pRxBuffer;

   //if est 1
    //pTxBuffer[0] = 0x11;

    iLoopCount= 0;
    CLR_YLED();
    // Turn on RX mode
    basicRfReceiveOn();

	// The main loop:
	while (TRUE) {

        switch(estado){
            case 1: //Recibimos un START_REQUEST
                    enviar_trama(START_RESPONSE);
                    //wait_response(100);
                    //wait_response(10);
                    while (iclk<10000){
                    halWait(20000);
                    iclk++;
                    if(estado == 2)
                        break;
                    }
                    iclk = 0;
                    break;
            case 2: //Recibimos un ACK
                wait=FALSE;
                //wait_response(60000); //ESPERAMOS SLEEP_REQUEST
                break;
            case 3: //Recibimos un SLEEP_REQUEST
                wait=FALSE;
                enviar_trama(SLEEP_RESPONSE);
                iclk=0;
                while (iclk<10000){
                                    halWait(20000);
                                    iclk++;
                                    if(estado == 4)
                                        break;
                                    }
                                    iclk = 0;
                //wait_response(100);
                 //ESPERAMOS ACK
                break;
            case 4: //RECIBIMOS EL ACK
                wait=FALSE;
                rfshutdown();
                enviar_trama(WAKE_UP);
                //wait_response(200); //ESPERAMOS ACK
                break;
            case 5: //Recibimos el ACK
                wait=FALSE;
                //wait_response(200); //ESPERAMOS DATA REQUEST
                break;
            case 6: //RECIBIMOS EL DATA REQUEST
                wait=FALSE;
                enviar_trama(DATA);
                break;
        }
	}

 // main
}


/*******************************************************************************************************
 * Peripheral functions of the main...
 *******************************************************************************************************/
void InitP2_7(void){
	P2DIR &= 0X7F;
	P2SEL &= BIT7;
	P2IES |= 0x80;
	P2IE |= BIT7;
	_EINT();
}
/****************************************************************************************************
 * fUNCTION BASIC_RF_INIT
 * //-----------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------
 *******************************************************************************************************/
void basicRfInit(BASIC_RF_RX_INFO *pRRI, UINT8 channel, UINT16 panId, UINT16 myAddr){
    UINT8 n;

    //Make sure that the voltage regulator is on, and that the reset pin is inactive
    SET_VREG_ACTIVE();
    halWait(1000);
    SET_RESET_ACTIVE();
    halWait(1000);
    SET_RESET_INACTIVE();
    halWait(500);
    //DISABLE_GLOBAL_INT();
    // Register modifications
    FASTSPI_STROBE(CC2420_SXOSCON);
    FASTSPI_SETREG(CC2420_MDMCTRL0, 0x0AF2); // Turn on automatic packet acknowledgment
    FASTSPI_SETREG(CC2420_MDMCTRL1, 0x0500); // Set the correlation threshold = 20
    FASTSPI_SETREG(CC2420_IOCFG0, 0x007F);   // Set the FIFOP threshold to maximum
    FASTSPI_SETREG(CC2420_SECCTRL0, 0x01C4); // Turn off "Security enable"

    // Set the RF channel
    halRfSetChannel(channel);

    // Turn interrupts back on
	//ENABLE_GLOBAL_INT();

	// Set the protocol configuration
	rfSettings.pRxInfo = pRRI;
	rfSettings.panId = panId;
	rfSettings.myAddr = myAddr;
	rfSettings.txSeqNumber = 0;
    rfSettings.receiveOn = FALSE;




	// Wait for the crystal oscillator to become stable
    halRfWaitForCrystalOscillator();

	// Write the short address and the PAN ID to the CC2420 RAM (requires that the XOSC is on and stable)
   	DISABLE_GLOBAL_INT();
    FASTSPI_WRITE_RAM_LE(&myAddr, CC2420RAM_SHORTADDR, 2, n);
    FASTSPI_WRITE_RAM_LE(&panId, CC2420RAM_PANID, 2, n);
    _EINT();
}

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
BOOL basicRfSendPacket(BASIC_RF_TX_INFO *pRTI)
{
	UINT16 frameControlField;
    UINT8 packetLength;
    BOOL success;
    UINT8 spiStatusUINT8;

    // Wait until the transceiver is idle
    while (FIFOP_IS_1 || SFD_IS_1);

    // Turn off global interrupts to avoid interference on the SPI interface
    DISABLE_GLOBAL_INT();

	// Flush the TX FIFO just in case...
	FASTSPI_STROBE(CC2420_SFLUSHTX);

    // Turn on RX if necessary
    if (!rfSettings.receiveOn) FASTSPI_STROBE(CC2420_SRXON);

    // Wait for the RSSI value to become valid
    do {
        FASTSPI_UPD_STATUS(spiStatusUINT8);
    } while (!(spiStatusUINT8 & BM(CC2420_RSSI_VALID)));

    packetLength = pRTI->length + BASIC_RF_PACKET_OVERHEAD_SIZE;
    frameControlField = pRTI->ackRequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;


    FASTSPI_WRITE_FIFO((UINT8*)&packetLength, 1);             // Packet length
    FASTSPI_WRITE_FIFO((UINT8*)&frameControlField, 2);         // Frame control field
    FASTSPI_WRITE_FIFO((UINT8*)&rfSettings.txSeqNumber, 1);    // Sequence number
    FASTSPI_WRITE_FIFO((UINT8*)&rfSettings.panId, 2);          // Dest. PAN ID
    FASTSPI_WRITE_FIFO((UINT8*)&pRTI->destAddr, 2);            // Dest. address
    FASTSPI_WRITE_FIFO((UINT8*)&rfSettings.myAddr, 2);         // Source address
	FASTSPI_WRITE_FIFO((UINT8*)pRTI->pPayload, pRTI->length);  // Payload

	// Wait for the transmission to begin before exiting (makes sure that this
    // function cannot be calld a second time, and thereby cancelling the first
    // transmission (observe the FIFOP + SFD test above).

	FASTSPI_STROBE(CC2420_STXONCCA);
 
   while (!SFD_IS_1);

	// Turn interrupts back on
	ENABLE_GLOBAL_INT();

    // Wait for the acknowledge to be received, if any
    if (pRTI->ackRequest) {
		rfSettings.ackReceived = FALSE;

		// Wait for the SFD to go low again
		while (SFD_IS_1);

        // We'll enter RX automatically, so just wait until we can be sure that the
        // ack reception should have finished. The timeout consists of a 12-symbol
        // turnaround time, the ack packet duration, and a small margin
        halWait((12 * BASIC_RF_SYMBOL_DURATION) + (BASIC_RF_ACK_DURATION) +
                (2 * BASIC_RF_SYMBOL_DURATION) + 100);

		// If an acknowledgment has been received (by the FIFOP interrupt),
        // the ackReceived flag should be set
		success = rfSettings.ackReceived;
    } else {
        success= TRUE;
    }

	// Turn off the receiver if it should not continue to be enabled
    DISABLE_GLOBAL_INT();
	if (!rfSettings.receiveOn) FASTSPI_STROBE(CC2420_SRFOFF);
    ENABLE_GLOBAL_INT();

    // Increment the sequence number, and return the result
    rfSettings.txSeqNumber++;
    return success;

} // halRfSendPacket

//-----------------------------------------------------------------------------------
//  void halRfReceiveOn(void)
//
//  DESCRIPTION:
//      Enables the CC2420 receiver and the FIFOP interrupt. When a packet is received through this
//      interrupt, it will call halRfReceivePacket(...), which must be defined by the application
//-----------------------------------------------------------------------------------
void basicRfReceiveOn(void)
{
    rfSettings.receiveOn = TRUE;
	FASTSPI_STROBE(CC2420_SRXON);
	FASTSPI_STROBE(CC2420_SFLUSHRX);
	FIFOP_INT_INIT();
    ENABLE_FIFOP_INT();
} // basicRfReceiveOn




//-----------------------------------------------------------------------------------
//  void halRfReceiveOff(void)
//
//  DESCRIPTION:
//      Disables the CC2420 receiver and the FIFOP interrupt.
//-----------------------------------------------------------------------------------
void basicRfReceiveOff(void)
{
    rfSettings.receiveOn = FALSE;
	FASTSPI_STROBE(CC2420_SRFOFF);
    DISABLE_FIFOP_INT();
} // basicRfReceiveOff


// Deep Sleep
void rfshutdown(void)
{
unsigned int ing = 0;
SET_RESET_ACTIVE();
SET_VREG_INACTIVE();
SPI_DISABLE();
//wait_response(900);

while (ing<10000){
halWait(20000);
ing++;
}
SPI_ENABLE();
SET_VREG_ACTIVE();
}






//***********************************************************************************************************
//***********************************************************************************************************
//interrupt handler
// PORT 1
// PORT 2
//***********************************************************************************************************
//***********************************************************************************************************
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR(void){
	
	if((P2IFG && 0x80)==1){
		//__low_power_mode_4();
		halWait(100);
		//__low_power_mode_off_on_exit();  // MSP430 SLEEP
		P2IFG &= 0x7F;//clear flag from user button
		//switch off the LEDs...
		CLR_YLED();
		CLR_BLED();
		CLR_RLED();
		bucleInicial = FALSE;//exit from the initial loop
	}
	//__low_power_mode_off_on_exit()
	
}
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

#pragma vector=PORT1_VECTOR
__interrupt void fifo_rx(void){
	UINT16 frameControlField;
	INT8 length;
	UINT8 pFooter[2];
	
	TOGGLE_RLED();            //INTERUPPT ACTIVE
    CLEAR_FIFOP_INT();


    // Clean up and exit in case of FIFO overflow, which is indicated by FIFOP = 1 and FIFO = 0
	if((FIFOP_IS_1) && (!(FIFO_IS_1))) {
	    FASTSPI_STROBE(CC2420_SFLUSHRX);
	    FASTSPI_STROBE(CC2420_SFLUSHRX);
	    return;
	}

	// Payload length
	FASTSPI_READ_FIFO_UINT8(length);

	length &= BASIC_RF_LENGTH_MASK; // Ignore MSB

    // Ignore the packet if the length is too short
    if (length < BASIC_RF_ACK_PACKET_SIZE) {
    	FASTSPI_READ_FIFO_GARBAGE(length);

        // Otherwise, if the length is valid, then proceed with the rest of the packet
    } else {
        // Register the payload length
        rfSettings.pRxInfo->length = length - BASIC_RF_PACKET_OVERHEAD_SIZE;

        // Read the frame control field and the data sequence number
        FASTSPI_READ_FIFO_NO_WAIT((UINT8*) &frameControlField, 2);
        rfSettings.pRxInfo->ackRequest = !!(frameControlField & BASIC_RF_FCF_ACK_BM);
    	FASTSPI_READ_FIFO_UINT8(rfSettings.pRxInfo->seqNumber);

		// Is this an acknowledgment packet?
    	if ((length == BASIC_RF_ACK_PACKET_SIZE) && (frameControlField == BASIC_RF_ACK_FCF)
            && (rfSettings.pRxInfo->seqNumber == rfSettings.txSeqNumber)) {

 	       	// Read the footer and check for CRC OK
			FASTSPI_READ_FIFO_NO_WAIT((UINT8*) pFooter, 2);

			// Indicate the successful ack reception (this flag is pol by the transmission routine)
			if (pFooter[1] & BASIC_RF_CRC_OK_BM) rfSettings.ackReceived = TRUE;

            // Too small to be a valid packet?
		} else if (length < BASIC_RF_PACKET_OVERHEAD_SIZE) {
			FASTSPI_READ_FIFO_GARBAGE(length - 3);
			return;

            // Receive the rest of the packet
		} else {

			// Skip the destination PAN and address (that's taken care of by harware address recognition!)
			FASTSPI_READ_FIFO_GARBAGE(4);

			// Read the source address
			FASTSPI_READ_FIFO_NO_WAIT((UINT8*) &rfSettings.pRxInfo->srcAddr, 2);

			// Read the packet payload
			FASTSPI_READ_FIFO_NO_WAIT(rfSettings.pRxInfo->pPayload, rfSettings.pRxInfo->length);

			// Read the footer to get the RSSI value
			FASTSPI_READ_FIFO_NO_WAIT((UINT8*) pFooter, 2);
			rfSettings.pRxInfo->rssi = pFooter[0];

			if(rfSettings.pRxInfo->pPayload[0] == START_REQUEST && estado == 0){
			    estado = 1;
			}
			else if(estado == 1 && rfSettings.pRxInfo->pPayload[0]== ACK){
			    estado = 2;
                            wait=FALSE;
			}
			else if(rfSettings.pRxInfo->pPayload[0] == SLEEP_REQUEST){
			    estado = 3;
                            wait=FALSE;
			}
			else if(estado == 3 && rfSettings.pRxInfo->pPayload[0]== ACK){
                            estado = 4;
                            wait=FALSE;
            }
            else if(estado == 4 && rfSettings.pRxInfo->pPayload[0]== ACK){
                estado = 5;
                wait=FALSE;
            }
            else if(rfSettings.pRxInfo->pPayload[0]== DATA_REQUEST){
                estado = 6;
                wait=FALSE;
            }
			x = rfSettings.pRxInfo->rssi - 45;

			// Notify the application about the received _data_ packet if the CRC is OK
			//if (((frameControlField & (BASIC_RF_FCF_BM)) == BASIC_RF_FCF_NOACK) && (pFooter[1] & BASIC_RF_CRC_OK_BM)) {
			//	rfSettings.pRxInfo= basicRfReceivePacket(rfSettings.pRxInfo);
			//}
			recibidos++;
		}
    }

} // SIGNAL(SIG_INTERRUPT0)

void wait_response(int temps){
    clock_t start, end;
    double time_used;
    wait=TRUE;
    start = clock();
    while(wait){
        end = clock();
        time_used += ((double) (end - start)) / CLOCKS_PER_SEC;
        if(time_used*1000 >= temps){ //Si pasa mas de 200 milis dejamos de esperar y abortamos comunicacion.
            wait = FALSE;
            if(estado==5){
                estado=4;
            }
        }
    }
}

void enviar_trama(UINT8 data){
    rfTxInfo.destAddr = DESADDR;
    rfTxInfo.ackRequest = FALSE;
    rfTxInfo.destPanId = PANID;
    rfTxInfo.length = PAYLOAD_SIZE;
    rfTxInfo.pPayload[0] = data;
    for (i=1; i<10; i++){
        rfTxInfo.pPayload[i] = 0xEE;
    }

    if (basicRfSendPacket(&rfTxInfo))
   {
       // OK -> Blink the LED fast
       //ledFlash(10, 5000);
       TOGGLE_BLED();
   } else {

       // No acknowledgment received -> Blink the LED slow
       //ledFlash(5, 10000);
       TOGGLE_RLED();
   }

}
