// EZ-Chat
// Brandon Elliott / Harman Grewal
// June 2009
/********************************************************************************************
Chat Program -- Link Listen
**************************************************************************************************/
#include "bsp.h"
#include "mrfi.h"
#include "nwk_types.h"
#include "nwk_api.h"
#include "bsp_leds.h"
#include "bsp_buttons.h"

char name[11] = "Sam: ";
char Buffer[50] = {0};
static void linkFrom(void);
void DisplayCharacter(char[], char);
volatile char Temp_Size, Temp_General=0;
static          uint8_t  sRxTid;
static          linkID_t sLinkID2;
static volatile uint8_t  sSemaphore;
char Key =0;
char Key_Not_Pressed, TxHold = 1;
static uint8_t sRxCallback(linkID_t);
void tx_char(uint8_t *, int*);
void KeyPressEnable(void);
void Get_Name (void);
#define DELAY  NWK_DELAY(100)


void main (void){
  
  // SimpliciTI Initialization
  BSP_Init();
  SMPL_Init(sRxCallback);
  
  // Display message and wait for key press
  KeyPressEnable();
  char opening_msg[]= "press key to link\r\n\r\n "; 
  Temp_Size = sizeof(opening_msg);
  DisplayCharacter(opening_msg, Temp_Size);
  while((Key_Not_Pressed));
  
  // Go to main process
  linkFrom();
  while (1);
}

static void linkFrom(){
  uint8_t  msg[2]={0};
  int count = 0;
  
  // listen for link forever... 
  while (1)
  {
    if (SMPL_SUCCESS == SMPL_LinkListen(&sLinkID2))
    {
      break;
    }
    // Implement fail-to-link policy here. otherwise, listen again. 
  }
  char array[]= "CONNECTED - Listen\r\n\r\n"; // We are Connected, Display message
  Temp_Size = sizeof(array);
  DisplayCharacter(array, Temp_Size);
  SMPL_Ioctl( IOCTL_OBJ_RADIO, IOCTL_ACT_RADIO_RXON, 0);
  
  KeyPressEnable();
  int * countptr;
  countptr = &count;
  uint8_t* char_var;
  char_var = &msg[0];
  uint8_t  rxmsg[2], tid = 0;
  while(1){                                 // Main Loop

    tx_char(char_var, countptr);            // Transmit function
    if (sSemaphore)                         // Poll for Received Msgs
     {
       *(rxmsg+1) = ++tid;
       SMPL_Send(sLinkID2, rxmsg, 2);
       sSemaphore = 0;
     }
  }
}


void tx_char(uint8_t  *msg_ptr, int *count){
  uint8_t  rxmsg[2], tid = 0;
  
  while(Key_Not_Pressed);                   // Wait for a key press from the user
  if( *count >35) TxHold = 0;               // Overflow Condition
  
  Buffer[*count] = Key;                     // Queue the character
  (*count)++;
  if (*count == 1){
    int Temp_Size = sizeof(name);
    DisplayCharacter(name, Temp_Size);
  }
  char array[1]= {Key};
  DisplayCharacter(array, 1);               // Echo the character
  
  if (sSemaphore){                          // If msg received while typing
    
    *(rxmsg+1) = ++tid;
    SMPL_Send(sLinkID2, rxmsg, 2);
    sSemaphore = 0;
  }
  if (!TxHold){                             // At this point we're ready to tx
    Buffer[*count] = 0x12;
    char queue[1];    
    for(int j=0; j<10; j++){                // Send your name
      
      queue[0] = name[j];
      SMPL_Send(sLinkID2, queue, 2);}
    for(int i=0; i< *count; i++)            // Send your message
    {
      msg_ptr[0] = Buffer[i];
      SMPL_Send(sLinkID2, msg_ptr, 2);
    }
    TxHold=1;
    *count=0;
    char array5[]= "\r\n\r\n";
    int Temp_Size = sizeof(array5);
    DisplayCharacter(array5, Temp_Size);
    
  }
  
  Key_Not_Pressed = 1;
  
  
}

/* handle received messages */
static uint8_t sRxCallback(linkID_t port){
  uint8_t msg[2], len, tid;
  if (port == sLinkID2)
  {
    if ((SMPL_SUCCESS == SMPL_Receive(sLinkID2, msg, &len)) && len)
    {
        if(msg[0] == 0x0D){
        char array3[]= "\r\n\r\n";
        Temp_Size = sizeof(array3);
        DisplayCharacter(array3, Temp_Size);
     }
      if(msg[0]< 0x80)
        DisplayCharacter(msg, 1);
      /* Check the application sequence number to detect
      late or missing frames...*/
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
  _DINT();                                  // This function enables the UART
  P3SEL |= 0x30;                            // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x41;                           // 8MHz 9600
  UCA0BR1 = 0x03;                           // 8MHz 9600
  UCA0MCTL = UCBRS1;                        // Modulation UCBRSx = 2
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  for(int count=0; count<size; count++){
    while (!(IFG2&UCA0TXIFG));              // USCI_A0 TX buffer ready?
    UCA0TXBUF = array[count];               // TX -> RXed character
    _EINT();
  }
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

// Set up USCI channel A for back-channel UART
void KeyPressEnable(void){ 
  UCA0CTL1 |= UCSWRST;                       // Reset State Machine
  UCA0CTL1 &= ~UCSWRST;                      // Init USCI Ch. A for key Press
  P3SEL |= 0x30;                             // P3.4,5 = USCI_A0 TXD/RXD
  UCA0CTL1 |= UCSSEL_2;                      // SMCLK
  UCA0BR0 = 0x41;                            // 8MHz / 9600
  UCA0BR1 = 0x03;                            // 8MHz / 9600 = 1041.6
  UCA0MCTL = UCBRS1;                         // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                      // Initialize USCI state machine
  IE2 |= UCA0RXIE;                           // Enable USCI_A0 RX interrupt
  Key_Not_Pressed = 1;
  
}
