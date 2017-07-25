// Receiver Sketch for Dehumidifier controller project
// Written and copyright by A.G.Doswell 08 Aug 2014, modified for output status 16 Dec 2014
// Receiver connected to pin 9


#include <VirtualWire.h>
#include <VirtualWire_Config.h>

int TX_ID =10;
int Temp;
int Humidity;
int Dew;
boolean Output;
char buffer[5];
int led = 13;


void setup() {
              pinMode(led, OUTPUT);   
              //Comms set up
              Serial.begin(9600);
              // Virtualwire setup
              vw_set_tx_pin(10); // Even though it's not used, we'll define it so the default doesn't interfere with the LCD library.
              vw_set_rx_pin(9); // RX pin set to pin 9
              vw_setup(300); //sets virtualwire for a tx rate of 300 bits per second, nice and slow! 
              vw_rx_start();     
              }  
void loop()
{
typedef struct rxRemoteData //Defines the received data, this is the same as the TX struct
{
int    TX_ID; 
int    Temp;   
int    Humidity;
int    Dew;
boolean Output;

};

struct rxRemoteData receivedData;
uint8_t rcvdSize = sizeof(receivedData);

if (vw_get_message((uint8_t *)&receivedData, &rcvdSize)) 
{
  if (receivedData.TX_ID == 10)     { //Only if the TX_ID=10 do we process the data.
    int TX_ID = receivedData.TX_ID;
    int Temp = receivedData.Temp;
    int Humidity = receivedData.Humidity;
    int Dew = receivedData.Dew;
    boolean Output = receivedData.Output;
    digitalWrite(led, HIGH);
        
      // writes the received data to the serial interface
      
      Serial.print("#S|DATA|[");
      Serial.print(itoa((Humidity), buffer, 10));
      Serial.print(";");
      Serial.print(itoa((Temp), buffer, 10));
      Serial.print(";");
      Serial.print(itoa((Dew), buffer, 10));
      Serial.print(";");
      Serial.print(itoa((Output), buffer, 10));
      Serial.println("]#");
      delay (1000);
      digitalWrite(led, LOW);
 
      } 

  }
}

// End of file
