/*-----------------------------------------------------------------------------
	Agi Csaki (ac2289)
	Junia George (jag529)
	CS 3420 Final Project
	This file contains the AI against which a player must play poker
-----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "msp430x22x4.h"

/*-----------------------------------------------------------------------------
	Card Struct
		Represents a single card
		Suit: 0-3 representing spades, diamonds, clubs, hearts
		Value: 1-13 representing the value of the card
-----------------------------------------------------------------------------*/
typedef struct {
    unsigned int suit;
    unsigned int value;
} card;

/*-----------------------------------------------------------------------------
	Tiebreaker Struct
		Represents the outcome of a round
		Wincard: the highest card in the hand (can hold values 2-14)
		Rank: represents highest ranking poker hand the AI has (can hold values
		1-10 representing each unique type of poker hand)
-----------------------------------------------------------------------------*/
typedef struct {
    int wincard;
    int rank;
} tiebreaker;

/*-----------------------------------------------------------------------------
	Deck global variable
		Represents the deck of cards being used
		An array with 52 elements, ordering of card structs in the array 
		represents order of cards in the deck
		Invariant: each card struct in the deck is unique ensuring we simulate
		gameplay with a full deck of cards
-----------------------------------------------------------------------------*/
card* deck[52];

/*-----------------------------------------------------------------------------
	Player global variables
		Represent the hands of players 1 and 2
		An array with 2 elements representing the 2 cards each player holds
		Invariants: a card held by either player cannot also be contained in
		deck, and these arrays contain the same cards on either device
-----------------------------------------------------------------------------*/
card* player1[2];
card* player2[2];

/*-----------------------------------------------------------------------------
	Table global variable
		Represents the cards played by the dealer
		An array with 5 elements, representing the cards laid on the table by
		the dealer
		Invariants: a card laid on the table cannot also be contained in deck,
		and this array contains the same cards on both devices
-----------------------------------------------------------------------------*/
card* table[5];

/*-----------------------------------------------------------------------------
	Dealer global variable
		Represents which player is currently the dealer
		Can hold value 1 or 2, corresponding to either player
-----------------------------------------------------------------------------*/
unsigned int dealer = 1;

// Dummy variables to be used later in the program
unsigned int t = 0;
unsigned int j = 0;
unsigned int x = 0;
unsigned int y = 0;

/*-----------------------------------------------------------------------------
	Create Function
		Initialize a full deck of cards, enforcing the invariant that each
		card contained in the deck is unique
-----------------------------------------------------------------------------*/
void create() {
	for(x = 1; x <= 13; x++) {
		for(y = 0; y <=3; y++){
			deck[12*y+x-1] = (card*)malloc(sizeof(card*));
			deck[12*y+x-1]->value = x;
			deck[12*y+x-1]->suit = y;	
		}
	}
}

/*-----------------------------------------------------------------------------
	Shuffle Function
		Shuffle the deck of cards using the Fisher-Yates shuffle
		Maintains the invariant that the deck of cards contains 52 unique cards
-----------------------------------------------------------------------------*/
void shuffle()
{
	unsigned int i = 51;
    unsigned int j;
    card* temp;
	srand(time(NULL));
    while (i > 0){
    	j = rand() % (i + 1);
        temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
        i = i - 1;
    }
}

//flop
void flop(){
	table[0] = deck[4];
	table[1] = deck[5];
	table[2] = deck[6];
}

//turn
void turn(){
	table[3] = deck[8];	
}

//river
void river(){
	table[4] = deck[10];	
}

//input is an array of 5 cards in order of value (where aces have a value of 1)-> Out put is the type of hand you have
tiebreaker* pattern_match(card* hand[5]) {
	unsigned int isFlush = 1;
	unsigned int isStraight = 0;
	unsigned int foundAce = 0;
	
	unsigned int i = 1;
	unsigned int temp = hand[0]->suit;
	
	tiebreaker* win = (tiebreaker*)malloc(sizeof(tiebreaker*));
	win->rank = 0;
	win->wincard = 0;
	
	//check if flush-> set isFlush to 1 if it is
	while(isFlush == 1 && i < 5) {
		if(temp != hand[i]->suit) {
			isFlush = 0;
		}
		else i++;
		
	}	
	//check if straight
	if(hand[4]->value - hand[0]->value == 4) {
		isStraight = 1;
	}
	
	//check if there is an ace
	while(foundAce == 0 && j<5){
		if(hand[j]->value == 1){
			foundAce=1;
		}
	}
	
	//check if royalFlush
	if(hand[0]->value == 1 && (hand[4]->value - hand[1]->value == 3) && (hand[1]->value == 10) && (isFlush == 1)) {
		win->rank  = 10;
		win->wincard = 14;
		return win;
	}
	
	//straight flush
	if(isStraight == 1 && isFlush == 1) {
		win->rank  = 9;
		win->wincard = hand[4]->value;
		return win;
	}
	
	//flush
	if(isFlush){
		if(foundAce) {
			win->rank  = 6;
			win->wincard = 14;
			return win;
		}
		else{
			win->rank  = 6;
			win->wincard = hand[4]->value;
			return win;
		}
	}
	
	if(isStraight){
		if(hand[0]->value == 1 && (hand[4]->value - hand[1]->value == 3) && (hand[1]->value == 10)){
			win->rank  = 5;
			win->wincard = 14;
			return win;
		}
		win->rank  = 5;
		win->wincard = hand[4]->value;
		return win;
	}
	
	//all other hands
	if(hand[0]->value == hand[1]->value){
		if(hand[1]->value == hand[2]->value){
			if(hand[2]->value == hand[3]->value){
				//return 4 of a kind
				if(hand[0]->value == 1){
					win->rank  = 8;
					win->wincard = 14;
					return win;
				}
				win->rank  = 8;
				win->wincard = hand[0]->value;
				return win;
			}
			else if(hand[3]->value == hand[4]->value){
				//return full house
				if(hand[0]->value == 1){
					win->rank  = 7;
					win->wincard = 14;
					return win;
				}
				win->rank  = 7;
				win->wincard = hand[0]->value;
				return win;
			}
			else{
			//return triple
				if(hand[0]->value == 1){
					win->rank  = 4;
					win->wincard = 14;
					return win;
				}
				win->rank  = 4;
				win->wincard = hand[0]->value;
				return win;
			}
		}
		else if(hand[2]->value == hand[3]->value) {
			if(hand[3]->value == hand[4]->value){
				//return full house
				if(hand[2]->value == 1){
					win->rank  = 7;
					win->wincard = 14;
					return win;
				}
				win->rank  = 7;
				win->wincard = hand[0]->value;
				return win;
			}
			else{
				//return 2 pair
				if(hand[0]->value == 1 || hand[2]->value == 1){
					win->rank  = 3;
					if(hand[0]->value == 1){
						win->wincard = 1400 + hand[2]->value;
					}
					else win->wincard = 1400 + hand[0]->value;
					return win;
				}
				if(hand[0]->value < hand[2]->value){
					win->rank  = 3;
					win->wincard = hand[2]->value*100 + hand[0]->value;
					return win;
				}
				else {
					win->rank  = 3;
					win->wincard = hand[0]->value*100 + hand[2]->value;
					return win;
				}
			}
		}
		else if(hand[3]->value == hand[4]->value){
			//return 2 pair
			if(hand[0]->value == 1 || hand[3]->value == 1){
				win->rank  = 3;
				if(hand[0]->value == 1){
						win->wincard = 1400 + hand[3]->value;
					}
					else win->wincard = 1400 + hand[0]->value;
				return win;
			}
			if(hand[0]->value < hand[3]->value){
				win->rank  = 3;
				win->wincard = hand[3]->value*100 + hand[0]->value;
				return win;
			}
			else{
				win->rank  = 3;
				win->wincard = hand[0]->value*100 + hand[3]->value;
				return win;
			}
		}
		else{
			//return pair
			if(hand[0]->value == 1){
				win->rank  = 2;
				win->wincard = 14;
				return win;
			}
			win->rank  = 2;
			win->wincard = hand[0]->value;
			return win;
		}
	}
	else if (hand[1]->value == hand[2]->value){
		if(hand[2]->value == hand[3]->value){
			if(hand[3]->value == hand[4]->value){
				//return 4 of a kind
				if(hand[1]->value == 1){
					win->rank  = 8;
					win->wincard = 14;
					return win;
				}
				win->rank  = 8;
				win->wincard = hand[1]->value;
				return win;
			}
			else{
				//return triple
				if(hand[1]->value == 1){
					win->rank  = 4;
					win->wincard = 14;
					return win;
				}
				win->rank  = 4;
				win->wincard = hand[1]->value;
				return win;
			}
		}
		else if(hand[3]->value == hand[4]->value) {
				//return 2 pair
			if(hand[1]->value == 1 || hand[3]->value == 1){
				win->rank  = 3;
				if(hand[1]->value == 1){
					win->wincard = 1400 + hand[3]->value;
				}
				else win->wincard = 1400 + hand[1]->value;
				return win;
			}
			if(hand[1]->value < hand[3]->value){
				win->rank  = 3;
				win->wincard = hand[3]->value*100 + hand[1]->value;
				return win;
			}
			else{
				win->rank  = 3;
				win->wincard = hand[1]->value*100 + hand[3]->value;
				return win;
			}
		}
		else{
			//return pair
			if(hand[1]->value == 1){
				win->rank  = 2;
				win->wincard = 14;
				return win;
			}
			win->rank  = 2;
			win->wincard = hand[1]->value;
			return win;
		}
	}
	else if (hand[2]->value == hand[3]->value){
		if(hand[3]->value == hand[4]->value){
			//return triple
			if(hand[2]->value == 1){
				win->rank  = 4;
				win->wincard = 14;
				return win;
			}
			win->rank  = 4;
			win->wincard = hand[2]->value;
			return win;
		}
		else{
			//return pair
			if(hand[2]->value == 1){
				win->rank  = 2;
				win->wincard = 14;
				return win;
			}
			win->rank  = 2;
			win->wincard = hand[2]->value;
			return win;
		}
	}
	else if(hand[3]->value == hand[4]->value){
		//return pair
		if(hand[3]->value == 1){
			win->rank  = 2;
			win->wincard = 14;
			return win;
		}
		win->rank  = 2;
		win->wincard = hand[3]->value;
		return win;
	}
	else{
		if(foundAce){
			win->rank  = 1;
			win->wincard = 14;
			return win;
		}
		win->rank  = 1;
		win->wincard = hand[5]->value;
		return win;
	}
	return win;
}

//n is the number of cards that have been flipped on the table plus 2
tiebreaker* optimal(unsigned int n, card* inplay[7]){
	unsigned int i=0, j=0, a;
	card* hand[5];
	
	card* t;
	
	//sort inplay[] in ascending order
	for (i = 0; i < 7; ++i)
   {
        for (j = i + 1; j < 7; ++j)
        {
            if (inplay[i] > inplay[j])
            {
                t =  inplay[i];
                inplay[i] = inplay[j];
                inplay[j] = t;
            }
        }
    }
	
	tiebreaker* best;
	tiebreaker* temp;
	if(n == 5) {
		for(a = 0; a < 5; a ++){
			hand[a] = inplay[a];
		}
		best = pattern_match(hand);
	}
	else{
		for(x = 0; x < n; x++){
			if(n==7){
				for(y = 0; y < n; y++){
					if(x != y){
						while(j < n){
							if(i != x && i != y){
								hand[j] = inplay[i];	
								j++;
							}	
							i++;
						}
						temp = pattern_match(hand);
						if(temp->rank > best->rank){
							best = temp;
						}
						else if(temp->rank = best->rank){
							if(temp->wincard > best->wincard){
								best = temp;	
							}	
						}
					}	
				}
			}
			else {
				while(j < n){
					if(i != x){
						hand[j] = inplay[i];	
						j++;
					}	
					i++;
				}
				temp = pattern_match(hand);
				if(temp->rank > best->rank){
					best = temp;
				}
				else if(temp->rank = best->rank){
					if(temp->wincard > best->wincard){
						best = temp;	
					}	
				}	
			}
			
		}
	}
	
	return best;
}	

//n is the number of cards in play and p represents the player
tiebreaker* inplayarray(int p, int n){
	card* play[7];
	
	if(p == 1){
		memcpy(play, player1, 2 * sizeof(card)); 
	}
	else {
		memcpy(play, player2, 2 * sizeof(card));	
	}
	memcpy(play + 2, table, 5 * sizeof(card));
	return optimal(n, play);
}

/*------------------------------------------------------------------------
 * UART communication functions
 *----------------------------------------------------------------------*/

/* Initialize the UART for TX (9600, 8N1) */
/* Settings taken from TI UART demo */ 
void init_uart(void) {
	BCSCTL1 = CALBC1_1MHZ;        /* Set DCO for 1 MHz */
	DCOCTL  = CALDCO_1MHZ;
	P3SEL = 0x30;                 /* P3->4,5 = USCI_A0 TXD/RXD */
	UCA0CTL1 |= UCSSEL_2;         /* SMCLK */
	UCA0BR0 = 104;                /* 1MHz 9600 */
	UCA0BR1 = 0;                  /* 1MHz 9600 */
	UCA0MCTL = UCBRS0;            /* Modulation UCBRSx = 1 */
	UCA0CTL1 &= ~UCSWRST;         /* Initialize USCI state machine */
}

/* Transmit a single character over UART interface */
void uart_putc(char c) {
    while(!(IFG2 & UCA0TXIFG)); /* Wait for TX buffer to empty */
    UCA0TXBUF = c;				/* Transmit character */
}

/* Transmit a nul-terminated string over UART interface */
void uart_puts(char *str) {
	while (*str) {
		/* Replace newlines with \r\n carriage return */
		if(*str == '\n') { uart_putc('\r'); }
		uart_putc(*str++);
	}
}

/* Clear terminal screen using VT100 commands */
/* http://braun-home->net/michael/info/misc/VT100_commands->htm */
void uart_clear_screen(void) {
	uart_putc(0x1B);		/* Escape character */
 	uart_puts("[2J");		/* Clear screen */
 	uart_putc(0x1B);		/* Escape character */
 	uart_puts("[0;0H");		/* Move cursor to 0,0 */
}

/*-------------------------------------------------------------------------*/

void main(){
	init_uart();	/* Setup UART (Note: Sets clock to 1 MHz) */
	
	uart_puts("Creating deck of cards...\n\n");
	create();
	
	uart_puts("Shuffling deck of cards...\n\n");
	shuffle();
	
	if(dealer == 1){
		dealer = 2;
		player2[0] = deck[0];
		player2[1] = deck[2];
		player1[0] = deck[1];
		player1[1] = deck[3];
	}
	else {
		dealer = 1;
		player1[0] = deck[0];
		player1[1] = deck[2];
		player2[0] = deck[1];
		player2[1] = deck[3];
	}	
	
	
	//bet on cards (big blind small blind)
	
	//flop and bet
	
	//turn and bet
	
	//river	and bet
	
}
