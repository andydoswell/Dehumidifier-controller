

// 
// FILE:  wireless_dehumid.pde
// Written by Andy Doswell 24th May 2014
// PURPOSE: DHT11 sensor with LCD Display and output for controlling dehumidifier.
// Data send via virtualwire and 433MHZ link to remote display
// Dehumidifier is controlled by a relay attached to pin 7, defined in rlyPin.
// DHT11 sensor is connected to 8, as defined in #define
// LCD is a Hitachi HD44780 or compatible, attached thus:
// LCD RS pin to digital pin 12
// LCD Enable pin to digital pin 11
// LCD D4 pin to digital pin 5
// LCD D5 pin to digital pin 4
// LCD D6 pin to digital pin 3
// LCD D7 pin to digital pin 2
// RW to GND
// V0 Held high (this may or may not work with your LCD, usual practice is to connect this to a 10K pot between +5 and GND)
// TX connected to pin 10
// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
	double a = 17.271;
	double b = 237.7;
	double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
	double Td = (b * temp) / (a - temp);
	return Td;
}

#include <VirtualWire.h>
#include <VirtualWire_Config.h>

#include <dht.h>

dht DHT;

#define DHT11_PIN 8

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// define constants & global variables
const int rlyPin = 7; //defines pin the relay is connected to. relay is active low
const int TimerLength = 1800;// number of seconds in an hour / 2 - this sets the minimum time the dehumidifier will remain on for.
int Timer = 0; //timer is off to start
boolean DewFlag = false; // This is the dew flag. Off as default.
boolean OutputStatus = false; //Output status is off as default
int chk;
int Temp;
int Dew;
int Humidity;
int DewTrip;


// Data structure set up for Transmission

struct TXData
{
  int TX_ID;
  int Temp;
  int Humidity;
  int Dew;
  boolean Output;
};
  

void setup()
{
  lcd.begin(16, 2); // defines the LCD as a 16 x 2
  pinMode (rlyPin, OUTPUT); // sets our relay pin
  digitalWrite (rlyPin, HIGH); // sets the relay off for default condition.
  
  // Virtualwire setup
  vw_set_tx_pin(10); // TX pin set to 10
  vw_set_rx_pin(9); // RX pin set to a spare pin, even though we don't use it. If left as default it interferes with LCD operation.
  vw_setup(300); //sets virtualwire for a tx rate of 300 bits per second
  

}

void loop()
{
  for(int i = 0; i < 10 ; i++) { //transmit only every 9 loops (18 seconds)
    chk = DHT.read11(DHT11_PIN); // these 4 lines get data from the sensor
    Dew = dewPointFast(DHT.temperature, DHT.humidity);
    Temp = (DHT.temperature);
    Humidity = (DHT.humidity);
    DewTrip= Dew + 5; // Brings the dehumidifier on 5 deg C before the dew point. 
  
    // writes information about the system to the LCD
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
   
  // Dew detect loop. If the dewTrip point is reached, start the timer running and set the Dew flag
    if ( Temp <= DewTrip ) {
      DewFlag = true;
      Timer = 1;       
    } 
    else {
      DewFlag = false;
    }

  
    if (Timer >= TimerLength and DewFlag == false) { // If the timer has expired and there's no dew, switch the dehumidifier off.
      Timer = 0;
      digitalWrite (rlyPin, HIGH);
   
    }

    if (Timer !=0) {                // If the timer is running, switch the dehumidifier on , and write On to the lcd.
      digitalWrite (rlyPin, LOW);
      lcd.setCursor (13,0);
      lcd.print (" On");
      OutputStatus = true;
      Timer++;
    }
  
    else {
      lcd.setCursor (13,0);
      lcd.print ("off");
      OutputStatus = false;
    }
  
 delay (2000);
  }
  
struct TXData payload; //Loads the Data struct with the payload data
  
payload.TX_ID=10;
payload.Temp = Temp;
payload.Humidity = Humidity;
payload.Dew = Dew;
payload.Output = OutputStatus;
vw_send((uint8_t *)&payload, sizeof(payload)); //Transmits the struct
vw_wait_tx(); //waits for the TX to complete

}  
  
//
// END OF FILE
//

