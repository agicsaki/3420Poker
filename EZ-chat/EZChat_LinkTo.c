// EZ-Chat
// Brandon Elliott / Harman Grewal
// June 2009
/********************************************************************************************
Chat Program -- Link to
**************************************************************************************************/

#include "bsp.h"
#include "mrfi.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"
//#include "app_remap_led.h"
smplStatus_t STATUS;
static uint8_t  sRxTid;
static linkID_t sLinkID1;
static uint8_t sRxCallback(linkID_t);
static volatile uint8_t  sSemaphore;
#define SPIN_ABOUT_A_SECOND  NWK_DELAY(1000)
#define DELAY  NWK_DELAY(100)

void DisplayCharacter(char[], char);
void KeyPressEnable(void);
static void linkTo(void);
volatile char Temp_Size, Temp_General=0;
void tx_char(uint8_t *, int*);
void rx_char(void);

char Key_Not_Pressed, TxHold = 1;
char Key, txing = 0;
char Buffer[50] = {0};
char name[11] = "Jim: ";
int new_msg, entering_name = 1;

void main (void){
  BSP_Init();
  SMPL_Init(sRxCallback);
  KeyPressEnable();
  char opening_msg[]= "press key to link\r\n\r\n "; 
  Temp_Size = sizeof(opening_msg);
  DisplayCharacter(opening_msg, Temp_Size);
  while((Key_Not_Pressed));
  linkTo();
  while (1);
}

static void linkTo(){
  
  uint8_t  msg[2], tid, rxmsg[2] = 0;
  
  while (SMPL_SUCCESS != SMPL_Link(&sLinkID1))
  {
    SPIN_ABOUT_A_SECOND;
  }
  char array[]= "CONNECTED - Link To\r\n\r\n"; // Connection Established, display msg to hyperterminal
  Temp_Size = sizeof(array);
  DisplayCharacter(array, Temp_Size);
  SMPL_Ioctl( IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_RXON, 0);
  int count = 0;
  KeyPressEnable();
  int * countptr;
  countptr = &count;
  uint8_t* char_var;
  char_var = &msg[0];
  DELAY;

  
  while (1)
  {
    tx_char(char_var, countptr); // Transmit
    
    if (sSemaphore)
    {
      *(rxmsg+1) = ++tid;
      SMPL_Send(sLinkID1, rxmsg, 2);
      /* Reset semaphore. This is not properly protected and there is a race
      * here. In theory we could miss a message. Good enough for a demo, though.
      */
      sSemaphore = 0;
    }
  }
}

void tx_char(uint8_t  *msg_ptr, int *count){
  uint8_t tid, rxmsg[2]=0;
  
    while((Key_Not_Pressed));  //Wait for a key press from the user
  if( *count >35){ TxHold = 0;
  //wait_message = 1;
  }

  Buffer[*count] = Key;
  (*count)++;
  if (*count == 1){
    int Temp_Size = sizeof(name);
    DisplayCharacter(name, Temp_Size);
  }
  char array[1]= {Key};
  DisplayCharacter(array, 1);
  
  if (sSemaphore)
  {
    *(rxmsg+1) = ++tid;
    SMPL_Send(sLinkID1, rxmsg, 2);
    sSemaphore = 0;
  }
  
  if (!TxHold){
    Buffer[*count] = 0x12;
    char queue[1];
    for(int j=0; j<10; j++){
    queue[0] = name[j];
    SMPL_Send(sLinkID1, queue, 2);}
    for(int i=0; i< *count; i++)
    { 
      msg_ptr[0] = Buffer[i];
      SMPL_Send(sLinkID1, msg_ptr, 2);
    }
    new_msg = 0;
    char array5[]= "\r\n";
    int Temp_Size = sizeof(array5);
    DisplayCharacter(array5, Temp_Size);
    TxHold=1;
    *count=0;
  }
  
  Key_Not_Pressed = 1;
    
}

/* handle received frames. */
static uint8_t sRxCallback(linkID_t port){ 
  uint8_t msg[2], len, tid;
  if (port == sLinkID1)
  {
    if ((SMPL_SUCCESS == SMPL_Receive(sLinkID1, msg, &len)) && len)
    {         
        if(msg[0] == 0x0D){ 
          new_msg = 1;
        char array2[]= "\r\n\r\n";
        Temp_Size = sizeof(array2);
        DisplayCharacter(array2, Temp_Size);
      }
      if(msg[0]< 0x80)
        DisplayCharacter(msg, 1);
      /* Check the application sequence number to detect
      * late or missing frames...*/
      
      tid = *(msg+1);
      if (tid)
      {
        if (tid > sRxTid)
        {
          sRxTid = tid;
        }
      }
      else
      {
        /* wrap case... */
        if (sRxTid)
        {
          sRxTid = tid;
        }
      }
      sSemaphore = 1;
      
      return 1;
    }
  }
  return 0;
  
}

void DisplayCharacter(char array[], char size){
  
  _DINT();
  P3SEL |= 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x41;                            // 8MHz 9600
  UCA0BR1 = 0x03;                              // 8MHz 9600
  UCA0MCTL = UCBRS1;                        // Modulation UCBRSx = 2
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  for(int count=0; count<size; count++){
    while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    UCA0TXBUF = array[count];                    // TX -> RXed character
    _EINT();
  }
}


void KeyPressEnable(void){ 
  UCA0CTL1 |= UCSWRST;                       // Reset State Machine
  UCA0CTL1 &= ~UCSWRST;                      // Initialize USCI Ch. A for key P
  P3SEL |= 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSSEL_2;                      // SMCLK
  UCA0BR0 = 0x41;                            // 8MHz / 9600
  UCA0BR1 = 0x03;                            // 8MHz / 9600 = 1041.6
  UCA0MCTL = UCBRS1;                         // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                      // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                           // Enable USCI_A0 RX interrupt
}

//  Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){ 
  
  Key_Not_Pressed = 0;

  Key = UCA0RXBUF;
  if(Key == 0x0D)
  { 
    //we've rx'ed a return, transmit all that's in the buffer
    TxHold = 0;
  }
}
