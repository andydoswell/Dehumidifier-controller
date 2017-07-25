
#include <VirtualWire.h>

// LED's
int ledPin = 13;

// RF Transmission container
char headerCharMsg[10];
int RXdata[5];

void setup() {
  Serial.begin(9600);
  Serial.println ("Init....");

  // VirtualWire 
  // Initialise the IO and ISR
  // Required for DR3100
  vw_set_ptt_inverted(true); 
  // Bits per sec
  vw_setup(2000);     
  vw_rx_start();
  // Start the receiver PLL running
} // END void setup

void loop(){

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
int h; 
for (h=0; h < 5; h++){

// Non-blocking
  if (vw_get_message(buf, &buflen)) 
  {
    int i;
    // Message with a good checksum received, load from buffer into character array. 
    for (i = 0; i < buflen; i++)
    {            
      // Fill Sensor1CharMsg Char array with corresponding 
      // chars from buffer.   
      headerCharMsg[i] = char(buf[i]);
    }
   headerCharMsg[buflen] = '\0'; // adds a /0 (null) to end of character array, to prevent issues with next 
                                 // message of different length

RXdata[h] = atoi(headerCharMsg); // Adds received message to  received message array message array
 for (int j = 0; j < 5; j++) {
  Serial.print (j);
  Serial.print (": ");
  Serial.println (RXdata[j]);
}
    }  
  }
 
}
