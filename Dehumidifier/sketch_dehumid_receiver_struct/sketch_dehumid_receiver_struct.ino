// Receiver Sketch for Dehumidifier controller project
// Written by A.G.Doswell 23 May 2014
// LCD is a Hitachi HD44780 or compatible, attached thus:
// LCD RS pin to digital pin 12
// LCD Enable pin to digital pin 11
// LCD D4 pin to digital pin 5
// LCD D5 pin to digital pin 4
// LCD D6 pin to digital pin 3
// LCD D7 pin to digital pin 2
// RW to GND
// V0 Held high (this may or may not work with your LCD, usual practice is to connect this to a 10K pot between +5 and GND)
// Receiver connected to pin 9


#include <VirtualWire.h>
#include <VirtualWire_Config.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int TX_ID =10;
int Temp;
int Humidity;
int Dew;
boolean Output;



void setup() {
              lcd.begin (16, 2);
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
    lcd.setCursor(13,0);
    lcd.print (" Rx");
    delay (100);
    int TX_ID = receivedData.TX_ID;
    int Temp = receivedData.Temp;
    int Humidity = receivedData.Humidity;
    int Dew = receivedData.Dew;
    boolean Output = receivedData.Output;
    
        
      // writes the received data to the LCD
  lcd.clear ();
  lcd.print("Humidity:");
  lcd.print(Humidity);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(Temp);
  lcd.print((char)0xDF);
  lcd.print("C");
  lcd.setCursor(6, 1);
  lcd.print("Dew:");
  lcd.print(Dew);
  lcd.print((char)0xDF);
  lcd.print("C");
  
  if (Output == true) {
      lcd.setCursor (13,0);
      lcd.print (" On");
    
  }
     else {
       lcd.setCursor (13,0);
       lcd.print ("off");

           }
  
      } 

  }
}

// End of file

