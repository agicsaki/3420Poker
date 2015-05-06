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

/* Useful #defines */
#define RED_RECEIVE_LED 0x01
#define GREEN_LED   0x02

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
	 *   If unsuccessful, turn on both LEDs and wait forever */
	status = MRFI_SetRxAddrFilter(address);	
	MRFI_EnableRxAddrFilter();
	if( status != 0) {
		P1OUT = RED_RECEIVE_LED | GREEN_LED;
		while(1);
	}
		
	/* Red and green LEDs are output, green starts on */
	P1DIR = RED_RECEIVE_LED | GREEN_LED;
	P1OUT = GREEN_LED;
	
	/* Turn on the radio receiver */
	MRFI_RxOn();
	
	/* Main loop just toggles the green LED with some delay */
	__bis_SR_register(GIE);
	while(1){
		sleep(60000);
		P1OUT ^= GREEN_LED;
	}
}


/* Function to execute upon receiving a packet
 *   Called by the driver when new packet arrives */
void MRFI_RxCompleteISR(void) {
	/* Read the received data packet */
	mrfiPacket_t	packet;
	MRFI_Receive(&packet);
	
	/* Read the value of the payload and react accoringly */

	if (packet.frame[9] == "fold") {

		// End game, the player has folded
	}

	if (packet.frame[9] == [0]) {

		//Player did not place a bet, continue game accordingly
	}

	/* Case that player placed a bet */
	else {

		//Display player's bet and continue game accordingly
	}
	 
	/* Toggle the red LED to signal that data has arrived */
	P1OUT ^= RED_RECEIVE_LED;
}

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
