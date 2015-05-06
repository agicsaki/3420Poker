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
    unsigned int value;
    unsigned int suit;
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
card s1 = {1, 0};
card s2 = {2, 0};
card s3 = {3, 0};
card s4 = {4, 0};
card s5 = {5, 0};
card s6 = {6, 0};
card s7 = {7, 0};
card s8 = {8, 0};
card s9 = {9, 0};
card s10 = {10, 0};
card s11 = {11, 0};
card s12 = {12, 0};
card s13 = {13, 0};

card c1 = {1, 1};
card c2 = {2, 1};
card c3 = {3, 1};
card c4 = {4, 1};
card c5 = {5, 1};
card c6 = {6, 1};
card c7 = {7, 1};
card c8 = {8, 1};
card c9 = {9, 1};
card c10 = {10, 1};
card c11 = {11, 1};
card c12 = {12, 1};
card c13 = {13, 1};

card d1 = {1, 2};
card d2 = {2, 2};
card d3 = {3, 2};
card d4 = {4, 2};
card d5 = {5, 2};
card d6 = {6, 2};
card d7 = {7, 2};
card d8 = {8, 2};
card d9 = {9, 2};
card d10 = {10, 2};
card d11 = {11, 2};
card d12 = {12, 2};
card d13 = {13, 2};

card h1 = {1, 3};
card h2 = {2, 3};
card h3 = {3, 3};
card h4 = {4, 3};
card h5 = {5, 3};
card h6 = {6, 3};
card h7 = {7, 3};
card h8 = {8, 3};
card h9 = {9, 3};
card h10 = {13, 3};
card h11 = {11, 3};
card h12 = {12, 3};
card h13 = {13, 3};



card* deck[] = {&s1, &s2, &s3, &s4, &s5, &s6, &s7, &s8, &s9, &s10, &s11, &s12, &s13, &c1, &c2, &c3, &c4, &c5, &c6, &c7, &c8, &c9, &c10, &c11, &c12, &c13, 
	&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8, &d9, &d10, &d11, &d12, &d13, &h1, &h2, &h3, &h4, &h5, &h6, &h7, &h8, &h9, &h10, &h11, &h12, &h13};

/*-----------------------------------------------------------------------------
	Player glo&bal variables
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


void tostring(char str[], int num)
{
    int i, rem, len = 4, n;
 
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

/*-----------------------------------------------------------------------------
	Create Function
		Initialize a full deck of cards, enforcing the invariant that each
		card contained in the deck is unique
-----------------------------------------------------------------------------*/
/*void create() {
	char val[4];
	unsigned int x = 0;
	unsigned int y = 0;
	for(x = 1; x <= 13; x++) {
		for(y = 0; y <=3; y++){
			//deck[12*y+x-1] = (card*) malloc(sizeof(card));
			deck[12*y+x-1]->value = x;
			deck[12*y+x-1]->suit = y;	
			tostring(val, x);
			uart_puts(val);
			uart_puts(",");
			tostring(val, y);
			uart_puts(val);
		}
	}
}*/

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
	unsigned int i=0, j=0, a, x, y;
	tiebreaker* best;
	tiebreaker* temp;
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




void main(){

	//royal flush
	card* rflush[] = {&s1, &s10, &s11, &s12, &s13};
	
	//straight flush
	card* sflush[] = {&s2, &s3, &s4, &s5, &s6};
	
	//4ofKind
	card* early4[] = {&s2, &d2, &h2, &c2, &s6};
	card* late4[] = {&s2, &d9, &h9, &c9, &s9};
	
	//full house
	card* latef[] = {&s2, &d2, &h6, &c6, &s6};
	card* earlyf[] = {&s2, &d2, &h2, &c9, &s9};
	
	//flush
	card* flush[] = {&s1, &s4, &s5, &s9, &s13}; 
	
	//straight
	card* straight[] = {&s2, &d3, &h4, &s5, &d6};
	
	//3ofKind
	card* late3[] = {&s2, &d2, &h6, &c6, &s6};
	card* mid3[] = {&s2, &d6, &h6, &c6, &s12};
	card* early3[] = {&s2, &d2, &h2, &c9, &s12};
	
	//2pr
	card* tp1[] = {&s2, &d2, &h6, &c6, &s11};
	card* tp2[] = {&s1, &d2, &h2, &c9, &s9};
	card* tp3[] = {&s2, &d2, &h6, &c9, &s9};
	
	//1pr
	card* op1[] = {&s2, &d2, &h6, &c9, &s11};
	card* op2[] = {&s1, &d2, &h2, &c9, &s11};
	card* op3[] = {&s2, &d3, &h6, &c6, &s9};
	card* op4[] = {&s1, &d2, &h6, &c9, &s9};
	
	//high card
	card* hcace[] = {&d1, &h4, &c5, &c9, &s13}; 
	card* hc[] = {&d2, &h4, &c5, &c9, &s13}; 
	
	
	tiebreaker* l;
	
	char val[4];
	unsigned int x;
	WDTCTL = WDTPW + WDTHOLD;			// Stop WDT
	
	
	
	init_uart();	/* Setup UART (Note: Sets clock to 1 MHz) */
	uart_clear_screen();

	uart_puts("Creating deck of cards...\n\n");
	

	uart_puts("Shuffling deck of cards...\n\n");
	shuffle();
	
	
	l = pattern_match(rflush);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(sflush);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(early4);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(late4);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(earlyf);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(latef);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	l = pattern_match(flush);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(straight);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	l = pattern_match(late3);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(mid3);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	l = pattern_match(early3);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(tp1);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	l = pattern_match(tp2);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(tp3);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	l = pattern_match(op1);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(op2);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	l = pattern_match(op3);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(op4);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	l = pattern_match(hcace);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n"); 
	
	l = pattern_match(hc);
	tostring(val, l->rank);
	printf(val);
	printf(", ");
	tostring(val, l->wincard);
	printf(val);
	printf("\n\n");
	
	
	
	
	/*if(dealer == 1){
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
	}	*/
	
	
	//bet on cards (big blind small blind)
	
	//flop and bet
	
	//turn and bet
	
	//river	and bet
	
	
}