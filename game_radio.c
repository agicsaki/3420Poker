/*-----------------------------------------------------------------------------
 	Agi Csaki (ac2289)
 	Junia George (jag529)
 	CS 3420 Final Project: Poker
 	Recieving player information and interacting with game state
 	This file is built upon the provided simple_receive_app.c provided by the 
 	 coursestaff of ECE3140 for this project
-----------------------------------------------------------------------------*/

#include "bsp.h"
#include "mrfi.h"
#include "radios/family1/mrfi_spi.h"
#include <stdlib.h>
#include <string.h>

/* Parameterized "sleep" helper function */
void sleep(unsigned int count) {
	int i;
	for (i = 0; i < 10; i++) {
		while(count > 0) {
			count--;
			__no_operation();
		}
	}
}

/* Main function for receive application */
void main(void) {
	/* Set a filter address for packets received by the radio
	 *   This should match the "destination" address of
	 *   the packets sent by the transmitter. */
	uint8_t address[] = {0xC0,0xA8,0x42,0x01};
	
	/* Filter setup return value: success or failure */
	unsigned char status;
	
	/* Perform board-specific initialization */
	BSP_Init();
	
	/* Initialize minimal RF interface, wake up radio */
	MRFI_Init();
	MRFI_WakeUp();
			
	/* Attempt to turn on address filtering
	 *   If unsuccessful wait forever */
	status = MRFI_SetRxAddrFilter(address);	
	MRFI_EnableRxAddrFilter();
	if( status != 0) {
		while(1);
	}

	/* Turn on the radio receiver */
	MRFI_RxOn();
	
	/* Main loop */
	__bis_SR_register(GIE);
	while(1){
		
		mrfiPacket_t 	packet;

		/* First byte of packet frame holds message length in bytes */
		packet.frame[0] = strlen("abcde") + 8;	/* Includes 8-byte address header */
		
		/* Next 8 bytes are addresses, 4 each for source and dest. */
		packet.frame[1] = 0xC0;		/* Destination 192 - 168 - 66 - ??? */
		packet.frame[2] = 0xA8;
		packet.frame[3] = 0x42;
		packet.frame[4] = 0x02;
		
		packet.frame[5] = 0x02;		/* Source */
		packet.frame[6] = 0x00;
		packet.frame[7] = 0x01;
		packet.frame[8] = 0x02;
		
		/* Remaining bytes are the message/data payload */
		strcpy( (char *) &packet.frame[9] , "abcde" );

		/* Transmit the packet over the radio */
		MRFI_Transmit(&packet , MRFI_TX_TYPE_FORCED);
		
		sleep(60000);
		sleep(60000);
		sleep(60000);
		
	}
}


/* Function to execute upon receiving a packet
 *   Called by the driver when new packet arrives */
void MRFI_RxCompleteISR(void) {
	/* Read the received data packet */
	mrfiPacket_t	packet;
	MRFI_Receive(&packet);
	
	/* Read the value of the payload and react accoringly */

	if (packet.frame[9] == 'f') {

		// End game, the player has folded
	}

	if (packet.frame[9] == 'c') {

		//Player did not place a bet, continue game accordingly
	}

	/* Case that player placed a bet */
	else {
		//Display player's bet and continue game accordingly
		int betValue;
		int sizeOfValue = packet.frame[1];
		if (sizeOfValue == 4) {
			betValue = (packet.frame[9] - '0')*10 + (packet.frame[10] - '0');
		}
		else {
			betValue = (packet.frame[9] - '0')*100 + (packet.frame[10] - '0')*10 + (packet.frame[11] - '0');
		}
		
	}
}
