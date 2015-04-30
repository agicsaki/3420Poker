#include <stdio.h>

typedef struct {
    int suit;
    int value;
} card;

typedef struct {
    int wincard;
    int rank;
} tiebreaker;

card deck[52];

void create() {
	for(int x = 1; x <= 13, x++) {
		for(int y = 0; y <=3; y++){
			card[12*y+x-1] = card{y, x};
		}
	}
}


tiebreaker hand(card hand[]) {
	unsigned int isFlush = 1;
	unsigned int isStraight = 0;
	unsigned int foundAce = 0;
	
	
	//check if flush. set isFlush to 1 if it is
	unsigned int i = 1;
	unsigned int temp = hand[0]->suit;
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
	unsigned int j = 0;
	while(foundAce == 0 && j<5){
		if(hand[j]->value == 1){
			foundAce==1;
		}
	}
	
	//check if royalFlush
	if(hand[0]->value == 1 && (hand[4]->value - hand[1]->value == 3) && (hand[1]->value == 10) && (isFlush == 1)) {
		return tiebreaker win = {14, 10};
	}
	
	//straight flush
	if(isStraight == 1 && isFlush == 1) {
		return tiebreaker win = {hand[4], 9};
	}
	
	//flush
	if(isFlush){
		if(foundAce) {
			return tiebreaker win = {14, 6};
		}
		else return tiebreaker win = {hand[4], 6};
	}
	
	if(isStraight){
		if(hand[0]->value == 1 && (hand[4]->value - hand[1]->value == 3) && (hand[1]->value == 10)){
			return tiebreaker win = {14, 5};
		}
		return tiebreaker win = {hand[4], 5};
	}
	
	//all other hands
	if(hand[0]->value == hand[1]->value){
		if(hand[1]->value == hand[2]->value){
			if(hand[2]->value == hand[3]->value){
				//return 4 of a kind
				if(hand[0]->value == 1){
					return tiebreaker win = {14, 8};
				}
				return tiebreaker win = {hand[0], 8};
			}
			else if(hand[3]->value == hand[4]->value){
				//return full house
				if(hand[0]->value == 1){
					return tiebreaker win = {14, 7};
				}
				return tiebreaker win = {hand[0], 7};
			}
			else{
			//return triple
				if(hand[0]->value == 1){
					return tiebreaker win = {14, 4};
				}
				return tiebreaker win = {hand[0], 4};
			}
		}
		else if(hand[2]->value == hand[3]->value) {
			if(hand[3]->value == hand[4]->value){
				//return full house
				if(hand[2]->value == 1){
					return tiebreaker win = {14, 7};
				}
				return tiebreaker win = {hand[0], 7};
			}
			else{
				//return 2 pair
				if(hand[0]->value == 1 || hand[2]->value == 1){
					return tiebreaker win = {14, 3};
				}
				if(hand[0]->value < hand[2]->value){
					return tiebreaker win = {hand[2], 3}
				}
				else return tiebreaker win = {hand[0], 3};
			}
		}
		else if(hand[3]->value == hand[4]->value){
			//return 2 pair
			if(hand[0]->value == 1 || hand[3]->value == 1){
				return tiebreaker win = {14, 3};
			}
			if(hand[0]->value < hand[3]->value){
				return tiebreaker win = {hand[3], 3}
			}
			else return tiebreaker win = {hand[0], 3};
		}
		else{
			//return pair
			if(hand[0]->value == 1){
				return tiebreaker win = {14, 2};
			}
			else return tiebreaker win = {hand[0], 2};
		}
	}
	else if (hand[1]->value == hand[2]->value){
		if(hand[2]->value == hand[3]->value){
			if(hand[3]->value == hand[4]->value){
				//return 4 of a kind
				if(hand[1]->value == 1){
					return tiebreaker win = {14, 8};
				}
				return tiebreaker win = {hand[1], 8};
			}
			else{
				//return triple
				if(hand[1]->value == 1){
					return tiebreaker win = {14, 4};
				}
				return tiebreaker win = {hand[1], 4};
			}
		}
		else if(hand[3]->value == hand[4]->value) {
			else{
				//return 2 pair
				if(hand[1]->value == 1 || hand[3]->value == 1){
					return tiebreaker win = {14, 3};
				}
				if(hand[1]->value < hand[3]->value){
					return tiebreaker win = {hand[3], 3}
				}
				else return tiebreaker win = {hand[1], 3};
			}
		}
		else{
			//return pair
			if(hand[1]->value == 1){
				return tiebreaker win = {14, 2};
			}
			else return tiebreaker win = {hand[1], 2};
		}
	}
	else if (hand[2]->value == hand[3]->value){
		if(hand[3]->value == hand[4]->value){
			//return triple
			if(hand[2]->value == 1){
				return tiebreaker win = {14, 4};
			}
			return tiebreaker win = {hand[2], 4};
		}
		else{
			//return pair
			if(hand[2]->value == 1){
				return tiebreaker win = {14, 2};
			}
			else return tiebreaker win = {hand[2], 2};
		}
	}
	else if(hand[3]->value == hand[4]->value){
		//return pair
		if(hand[3]->value == 1){
			return tiebreaker win = {14, 2};
		}
		else return tiebreaker win = {hand[3], 2};
	}
	else{
		if(foundAce){
			return tiebreaker win = {14, 1};
		}
		else return tiebreaker win = {hand[5], 1};
	}

}