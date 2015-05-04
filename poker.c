#include <stdio.h>

typedef struct {
    unsigned int suit;
    unsigned int value;
} card;

typedef struct {
    int wincard;
    int rank;
} tiebreaker;

unsigned int x = 0;
unsigned int y = 0;
card deck[52];

unsigned int t = 0;
unsigned int j = 0;

void main(){
	
}

void create() {
	for(x = 1; x <= 13; x++) {
		for(y = 0; y <=3; y++){
			deck[12*y+x-1].value = x;
			deck[12*y+x-1].suit = y;
			
		}
	}
}


tiebreaker hand(card hand[5]) {
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
		return win;
	}
	
	//straight flush
	if(isStraight == 1 && isFlush == 1) {
		win.rank  = 9;
		win.wincard = hand[4].value;
		return win;
	}
	
	//flush
	if(isFlush){
		if(foundAce) {
			win.rank  = 6;
			win.wincard = 14;
			return win;
		}
		else{
			win.rank  = 6;
			win.wincard = hand[4].value;
			return win;
		}
	}
	
	if(isStraight){
		if(hand[0].value == 1 && (hand[4].value - hand[1].value == 3) && (hand[1].value == 10)){
			win.rank  = 5;
			win.wincard = 14;
			return win;
		}
		win.rank  = 5;
		win.wincard = hand[4].value;
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
					return win;
				}
				win.rank  = 8;
				win.wincard = hand[0].value;
				return win;
			}
			else if(hand[3].value == hand[4].value){
				//return full house
				if(hand[0].value == 1){
					win.rank  = 7;
					win.wincard = 14;
					return win;
				}
				win.rank  = 7;
				win.wincard = hand[0].value;
				return win;
			}
			else{
			//return triple
				if(hand[0].value == 1){
					win.rank  = 4;
					win.wincard = 14;
					return win;
				}
				win.rank  = 4;
				win.wincard = hand[0].value;
				return win;
			}
		}
		else if(hand[2].value == hand[3].value) {
			if(hand[3].value == hand[4].value){
				//return full house
				if(hand[2].value == 1){
					win.rank  = 7;
					win.wincard = 14;
					return win;
				}
				win.rank  = 7;
				win.wincard = hand[0].value;
				return win;
			}
			else{
				//return 2 pair
				if(hand[0].value == 1 || hand[2].value == 1){
					win.rank  = 3;
					win.wincard = 14;
					return win;
				}
				if(hand[0].value < hand[2].value){
					win.rank  = 3;
					win.wincard = hand[2].value;
					return win;
				}
				else {
					win.rank  = 3;
					win.wincard = hand[0].value;
					return win;
				}
			}
		}
		else if(hand[3].value == hand[4].value){
			//return 2 pair
			if(hand[0].value == 1 || hand[3].value == 1){
				win.rank  = 3;
				win.wincard = 14;
				return win;
			}
			if(hand[0].value < hand[3].value){
				win.rank  = 3;
				win.wincard = hand[3].value;
				return win;
			}
			else{
				win.rank  = 3;
				win.wincard = hand[0].value;
				return win;
			}
		}
		else{
			//return pair
			if(hand[0].value == 1){
				win.rank  = 2;
					win.wincard = 14;
					return win;
			}
			win.rank  = 2;
			win.wincard = hand[0].value;
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
					return win;
				}
				win.rank  = 8;
				win.wincard = hand[1].value;
				return win;
			}
			else{
				//return triple
				if(hand[1].value == 1){
					win.rank  = 4;
					win.wincard = 14;
					return win;
				}
				win.rank  = 4;
				win.wincard = hand[1].value;
				return win;
			}
		}
		else if(hand[3].value == hand[4].value) {
				//return 2 pair
			if(hand[1].value == 1 || hand[3].value == 1){
				win.rank  = 3;
				win.wincard = 14;
				return win;
			}
			if(hand[1].value < hand[3].value){
				win.rank  = 3;
				win.wincard = hand[3].value;
				return win;
			}
			else{
				win.rank  = 3;
				win.wincard = hand[1].value;
				return win;
			}
		}
		else{
			//return pair
			if(hand[1].value == 1){
				win.rank  = 2;
				win.wincard = 14;
				return win;
			}
			win.rank  = 2;
			win.wincard = hand[1].value;
			return win;
		}
	}
	else if (hand[2].value == hand[3].value){
		if(hand[3].value == hand[4].value){
			//return triple
			if(hand[2].value == 1){
				win.rank  = 4;
					win.wincard = 14;
					return win;
			}
			win.rank  = 4;
			win.wincard = hand[2].value;
			return win;
		}
		else{
			//return pair
			if(hand[2].value == 1){
				win.rank  = 2;
				win.wincard = 14;
				return win;
			}
			win.rank  = 2;
			win.wincard = hand[2].value;
			return win;
		}
	}
	else if(hand[3].value == hand[4].value){
		//return pair
		if(hand[3].value == 1){
			win.rank  = 2;
			win.wincard = 14;
			return win;
		}
		win.rank  = 2;
		win.wincard = hand[3].value;
		return win;
	}
	else{
		if(foundAce){
			win.rank  = 1;
			win.wincard = 14;
			return win;
		}
		win.rank  = 1;
		win.wincard = hand[5].value;
		return win;
	}
	return win;
}
