/*-----------------------------------------------------------------------------
 	Agi Csaki (ac2289)
 	Junia George (jag529)
 	CS 3420 Final Project: Poker
 	Player's interactions with the game occur in this file
 	This file is built upon the provided simple_send_app.c provided by the 
 	 coursestaff of ECE3140 for this project
-----------------------------------------------------------------------------*/

#include "bsp.h"
#include "mrfi.h"
#include "radios/family1/mrfi_spi.h"
#include "msp430x22x4.h"
#include <string.h>

/* Useful #defines */
#define RED_SEND_LED 		0x01

/* Time to wait for fold or check action to be taken */
unsigned int LONG_THRESHOLD;

/* Time to debounce press of button */
unsigned int SHORT_THRESHOLD;

/* Number of cycles the button has been held down,
   Can have value up to SHORT_THRESHOLD */
unsigned int button_counter;

/* Number of cycles the button has NOT been held down
   Can have value up to LONG_THRESHOLD */
unsigned int check_counter;

/* Number of consecutive cycles the button has been held down 
   Can have value up to LONG_THRESHOLD */
unsigned int consec_button_counter;

/* Variable to indicate a change has already occured in button status */
unsigned int change_occured;

/* Variable to indicate the player has folded with value 1, 0 otherwise */
unsigned int player_folds;

/* Variable to track the value of player's bet, 0 if no bet has been placed */
unsigned int player_bets;

/*-----------------------------------------------------------------------------
	Construct a packet and sent it over the radio given the message to send
		 *  Packet frame structure:
		 *  ---------------------------------------------------
		 *  | Length (1B) | Dest (4B) | Source (4B) | Payload |
		 *  ---------------------------------------------------
-----------------------------------------------------------------------------*/
void transmitPacket( char msg[] ) {

		mrfiPacket_t 	packet;

		/* First byte of packet frame holds message length in bytes */
		packet.frame[0] = strlen(msg) + 8;	/* Includes 8-byte address header */
		
		/* Next 8 bytes are addresses, 4 each for source and dest. */
		packet.frame[1] = 0xC0;		/* Destination 192 - 168 - 66 - ??? */
		packet.frame[2] = 0xA8;
		packet.frame[3] = 0x42;
		packet.frame[4] = 0x01;
		
		packet.frame[5] = 0x02;		/* Source */
		packet.frame[6] = 0x00;
		packet.frame[7] = 0x01;
		packet.frame[8] = 0x02;
		
		/* Remaining bytes are the message/data payload */
		strcpy( (char *) &packet.frame[9] , msg );

		/* Transmit the packet over the radio */
		MRFI_Transmit(&packet , MRFI_TX_TYPE_FORCED);

		/* Toggle red LED after transmitting */
		P1OUT ^= RED_SEND_LED;

}

/*-----------------------------------------------------------------------------
	Main function
		Initializes all neccessary values for radio communication and player's
		switch interactions
-----------------------------------------------------------------------------*/
void main(void) {
	
	/* Set a filter address for packets received by the radio
	 *   This should match the "destination" address of
	 *   the packets sent by the transmitter. */
	uint8_t address[] = {0xC0,0xA8,0x42,0x02};
	
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
	
	/* Set red LED to output */
	P1DIR = RED_SEND_LED;
	P1OUT = RED_SEND_LED;
	
	__bis_SR_register(GIE);

	/* Set values of counters and status flags */
	button_counter = 0;
	change_occured = 0;
	check_counter = 0;
	consec_button_counter = 0;
	LONG_THRESHOLD = 60000;
	SHORT_THRESHOLD = 10;
	WDTCTL = WDTPW + WDTHOLD;
	
	/* Set values of player action status flags */
	player_folds = 0;
	player_bets = 0;

	/* Set values for switch (P1.2) */
	P1REN |= 0x04;
	P1SEL &= ~0x04;
	P1IN &= ~0x04;
	P1DIR &= ~0x04;
	P1OUT |= 0x04;
	
	/* Main (infinite) loop waiting for user input */
	while(1){
		
	}
}

/*-----------------------------------------------------------------------------
	Function to execute upon receiving a packet (called by the driver)
		Any packet indicates that the game is ready for the player's turn
		Waits for user input and transmits a packet with the player's response
		Possible outcomes:
			*If the player does nothing for LONG_THRESHOLD amount of time, they
			 have checked and a packet is transmitted
			*If the player holds the button down for LONG_THRESHOLD amount of
			 time, they have folded and a packet is transmitted
			*If the player presses the button several times, then does nothing
			 for LONG_THRESHOLD amount of time, the number of clicks * 10 is
			 their bet and a packet is transmitted
-----------------------------------------------------------------------------*/
void MRFI_RxCompleteISR( void ) {
	
	mrfiPacket_t	packet;
	MRFI_Receive(&packet);

	/* Wait for user input and send a packet based on user's actions */
	while (consec_button_counter < LONG_THRESHOLD && check_counter < LONG_THRESHOLD) {

		/* Increment button held down counters */
		button_counter++;
		consec_button_counter++;

		/* If button released: reset button counters,increment check counter */
		if ((P1IN & 0x04) == 0x04) {
			button_counter = 0;
			consec_button_counter = 0;
			change_occured = 0;
			check_counter++;
		}

		/* If button has been held down for SHORT_THRESHOLD, increase bet */
		if (button_counter > SHORT_THRESHOLD) {
			if (change_occured == 0) {
				player_bets += 10;
				change_occured = 1; // Ensure bet is increased once per press
			}
			button_counter = 0;
			check_counter = 0;
		}

	}

	/* If button was not pressed for long enough, find out if player placed 
	   a bet or wanted to check, and send a packet accordingly */
	if (check_counter >= LONG_THRESHOLD) {
		if (player_bets == 0) {
			transmitPacket("check");
		}
		else {
			if (player_bets < 100) {
				char msg[2];
				memcpy(msg, &player_bets, 2);
				transmitPacket(msg);
			}
			else {
				char msg[3];
				memcpy(msg, &player_bets, 3);
				transmitPacket(msg);
			}
		}
	}

	/* If button was pressed for LONG_THRESHOLD time, player has folded */
	if (consec_button_counter >= LONG_THRESHOLD) {
		transmitPacket("fold");
	}

	/* Reset interaction indicators */
	player_bets = 0;
	player_folds = 0;
	consec_button_counter = 0;
	check_counter = 0;
	button_counter = 0;
	change_occured = 0;

}

