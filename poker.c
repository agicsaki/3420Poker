#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "msp430x22x4.h"

typedef struct {
    unsigned int suit;
    unsigned int value;
} card;

typedef struct {
    int wincard;
    int rank;
    card winhand[5];
} tiebreaker;

unsigned int x = 0;
unsigned int y = 0;
card deck[52];
card player1[2];
card player2[2];
card table[5];

unsigned int t = 0;
unsigned int j = 0;

unsigned int dealer = 1;

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


//create a deck of cards
void create() {
	for(x = 1; x <= 13; x++) {
		for(y = 0; y <=3; y++){
			deck[12*y+x-1].value = x;
			deck[12*y+x-1].suit = y;	
		}
	}
}

//shuffle deck of cards; this is the fisher-yates shuffle (a method I found online)
void shuffle()
{
	unsigned int i = 51;
    unsigned int j;
    card temp;
	srand(time(NULL));
    while (i > 0){
    	j = rand() % (i + 1);
        temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
        i = i - 1;
    }
}

//input is an array of 5 cards in order of value (where aces have a value of 1). Out put is the type of hand you have
tiebreaker pattern_match(card hand[5]) {
	unsigned int isFlush = 1;
	unsigned int isStraight = 0;
	unsigned int foundAce = 0;
	
	tiebreaker win = {0, 0};
	
	//check if flush. set isFlush to 1 if it is
	unsigned int i = 1;
	unsigned int temp = hand[0].suit;
	while(isFlush == 1 && i < 5) {
		if(temp != hand[i].suit) {
			isFlush = 0;
		}
		else i++;
		
	}	
	//check if straight
	if(hand[4].value - hand[0].value == 4) {
		isStraight = 1;
	}
	
	//check if there is an ace
	while(foundAce == 0 && j<5){
		if(hand[j].value == 1){
			foundAce=1;
		}
	}
	
	//check if royalFlush
	if(hand[0].value == 1 && (hand[4].value - hand[1].value == 3) && (hand[1].value == 10) && (isFlush == 1)) {
		win.rank  = 10;
		win.wincard = 14;
		memcpy(win.winhand, hand, 5 * sizeof(card));
		return win;
	}
	
	//straight flush
	if(isStraight == 1 && isFlush == 1) {
		win.rank  = 9;
		win.wincard = hand[4].value;
		memcpy(win.winhand, hand, 5 * sizeof(card));
		return win;
	}
	
	//flush
	if(isFlush){
		if(foundAce) {
			win.rank  = 6;
			win.wincard = 14;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
		else{
			win.rank  = 6;
			win.wincard = hand[4].value;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
	}
	
	if(isStraight){
		if(hand[0].value == 1 && (hand[4].value - hand[1].value == 3) && (hand[1].value == 10)){
			win.rank  = 5;
			win.wincard = 14;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
		win.rank  = 5;
		win.wincard = hand[4].value;
		memcpy(win.winhand, hand, 5 * sizeof(card));
		return win;
	}
	
	//all other hands
	if(hand[0].value == hand[1].value){
		if(hand[1].value == hand[2].value){
			if(hand[2].value == hand[3].value){
				//return 4 of a kind
				if(hand[0].value == 1){
					win.rank  = 8;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				win.rank  = 8;
				win.wincard = hand[0].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			else if(hand[3].value == hand[4].value){
				//return full house
				if(hand[0].value == 1){
					win.rank  = 7;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				win.rank  = 7;
				win.wincard = hand[0].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			else{
			//return triple
				if(hand[0].value == 1){
					win.rank  = 4;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				win.rank  = 4;
				win.wincard = hand[0].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
		}
		else if(hand[2].value == hand[3].value) {
			if(hand[3].value == hand[4].value){
				//return full house
				if(hand[2].value == 1){
					win.rank  = 7;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				win.rank  = 7;
				win.wincard = hand[0].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			else{
				//return 2 pair
				if(hand[0].value == 1 || hand[2].value == 1){
					win.rank  = 3;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				if(hand[0].value < hand[2].value){
					win.rank  = 3;
					win.wincard = hand[2].value;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				else {
					win.rank  = 3;
					win.wincard = hand[0].value;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
			}
		}
		else if(hand[3].value == hand[4].value){
			//return 2 pair
			if(hand[0].value == 1 || hand[3].value == 1){
				win.rank  = 3;
				win.wincard = 14;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			if(hand[0].value < hand[3].value){
				win.rank  = 3;
				win.wincard = hand[3].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			else{
				win.rank  = 3;
				win.wincard = hand[0].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
		}
		else{
			//return pair
			if(hand[0].value == 1){
				win.rank  = 2;
				win.wincard = 14;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			win.rank  = 2;
			win.wincard = hand[0].value;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
	}
	else if (hand[1].value == hand[2].value){
		if(hand[2].value == hand[3].value){
			if(hand[3].value == hand[4].value){
				//return 4 of a kind
				if(hand[1].value == 1){
					win.rank  = 8;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				win.rank  = 8;
				win.wincard = hand[1].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			else{
				//return triple
				if(hand[1].value == 1){
					win.rank  = 4;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
				}
				win.rank  = 4;
				win.wincard = hand[1].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
		}
		else if(hand[3].value == hand[4].value) {
				//return 2 pair
			if(hand[1].value == 1 || hand[3].value == 1){
				win.rank  = 3;
				win.wincard = 14;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			if(hand[1].value < hand[3].value){
				win.rank  = 3;
				win.wincard = hand[3].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			else{
				win.rank  = 3;
				win.wincard = hand[1].value;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
		}
		else{
			//return pair
			if(hand[1].value == 1){
				win.rank  = 2;
				win.wincard = 14;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			win.rank  = 2;
			win.wincard = hand[1].value;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
	}
	else if (hand[2].value == hand[3].value){
		if(hand[3].value == hand[4].value){
			//return triple
			if(hand[2].value == 1){
				win.rank  = 4;
					win.wincard = 14;
					memcpy(win.winhand, hand, 5 * sizeof(card));
					return win;
			}
			win.rank  = 4;
			win.wincard = hand[2].value;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
		else{
			//return pair
			if(hand[2].value == 1){
				win.rank  = 2;
				win.wincard = 14;
				memcpy(win.winhand, hand, 5 * sizeof(card));
				return win;
			}
			win.rank  = 2;
			win.wincard = hand[2].value;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
	}
	else if(hand[3].value == hand[4].value){
		//return pair
		if(hand[3].value == 1){
			win.rank  = 2;
			win.wincard = 14;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
		win.rank  = 2;
		win.wincard = hand[3].value;
		memcpy(win.winhand, hand, 5 * sizeof(card));
		return win;
	}
	else{
		if(foundAce){
			win.rank  = 1;
			win.wincard = 14;
			memcpy(win.winhand, hand, 5 * sizeof(card));
			return win;
		}
		win.rank  = 1;
		win.wincard = hand[5].value;
		memcpy(win.winhand, hand, 5 * sizeof(card));
		return win;
	}
	return win;
}

//n is the number of cards that have been flipped on the table plus 2
tiebreaker optimal(unsigned int n, card inplay[7]){
	unsigned int i=0, j=0, a;
	card hand[5];
	tiebreaker best, temp;
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
						if(temp.rank > best.rank){
							best = temp;
						}
						else if(temp.rank = best.rank){
							if(temp.wincard > best.wincard){
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
				if(temp.rank > best.rank){
					best = temp;
				}
				else if(temp.rank = best.rank){
					if(temp.wincard > best.wincard){
						best = temp;	
					}	
				}	
			}
			
		}
	}
	
	return best;
}	

//n is the number of cards in play and p represents the player
tiebreaker inplayarray(int p, int n){
	card play[7];
	
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
	P3SEL = 0x30;                 /* P3.4,5 = USCI_A0 TXD/RXD */
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
/* http://braun-home.net/michael/info/misc/VT100_commands.htm */
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
