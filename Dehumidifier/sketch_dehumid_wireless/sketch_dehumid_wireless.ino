

// 
// FILE:  dehumid.pde
// Written by Andy Doswell 4th May 2014
// PURPOSE: DHT11 sensor with LCD Display and output for controlling dehumidifier.
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
int WatchDog = 0; //watchdog default
const int TimerLength = 1800;// number of seconds in an hour / 2 - this sets the minimum time the dehumidifier will remain on for.
int Timer = 0; //timer is off to start
boolean DewFlag = false; // This is the dew flag. Off as default.
const int header = 10109;
char tempCharMsg[5];
char humidityCharMsg[5];
char OutputFlag[4];
char dewCharMsg[4];
char headerCharMsg[6];

void setup()
{
Serial.begin(9600);// for debug
  lcd.begin(16, 2); // defines the LCD as a 16 x 2
  pinMode (rlyPin, OUTPUT); // sets our relay pin
  digitalWrite (rlyPin, HIGH); // sets the relay off for default condition.
  vw_set_tx_pin(10); // TX pin set to 10
  vw_set_rx_pin(9); // RX pin set to a spare pin, even though we don't use it. If left as default it interferes with LCD operation.
  vw_setup(2000); //sets virtualwire for a tx rate of 2000 bits per second
  

}

void loop()
{
  int chk = DHT.read11(DHT11_PIN); // these 4 lines get data from the sensor
  int dew = dewPointFast(DHT.temperature, DHT.humidity);
  int temp = (DHT.temperature);
  int humidity = (DHT.humidity);
  int dewTrip= dew + 5; // Brings the dehumidifier on 5 deg C before the dew point. 
  
// writes information about the system to the LCD
  lcd.clear ();
  lcd.print("Humidity:");
  lcd.print(humidity);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(temp);
  lcd.print((char)0xDF);
  lcd.print("C");
  lcd.setCursor(6, 1);
  lcd.print("Dew:");
  lcd.print(dew);
  lcd.print((char)0xDF);
  lcd.print("C");
  
  // converts integers to strings so we can send them.
  
  
itoa(humidity,humidityCharMsg,10);
itoa(temp,tempCharMsg,10); 
itoa(dew,dewCharMsg,10);
itoa(header,headerCharMsg,10); 

// Transmits data to remote receiver  
vw_send((uint8_t *)headerCharMsg, strlen(headerCharMsg)); //transmits header to allow receiver to sync
vw_wait_tx(); // Wait until the whole message is gone
vw_send((uint8_t *)humidityCharMsg, strlen(humidityCharMsg)); //transmits humidity
vw_wait_tx(); // Wait until the whole message is gone
vw_send((uint8_t *)tempCharMsg, strlen(tempCharMsg)); //transmits temp
vw_wait_tx(); // Wait until the whole message is gone
vw_send((uint8_t *)dewCharMsg, strlen(dewCharMsg)); //transmits dew (value)
vw_wait_tx(); // Wait until the whole message is gone
vw_send((uint8_t *)OutputFlag, strlen(OutputFlag)); //transmits Dew (flag)
vw_wait_tx(); // Wait until the whole message is gone
Serial.print ("Header:");
Serial.println (headerCharMsg);
Serial.print ("Humidity data:");
Serial.println (humidity);
Serial.print ("Humidity character msg:");
Serial.println  (humidityCharMsg);
Serial.print ("Temp data:");
Serial.println (temp);
Serial.print ("Temp character msg:");
Serial.println  (tempCharMsg);
Serial.print ("Dew point data:");
Serial.println (dew);
Serial.print ("Dew point character msg:");
Serial.println  (dewCharMsg);



Serial.print ("Output Flag: ");
Serial.println (OutputFlag);

// Dew detect loop. If the dewTrip point is reached, start the timer running and set the Dew flag
  if ( temp <= dewTrip ) {
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
    strcpy (OutputFlag, "On");
    Timer++;
  }
  
 else {
   lcd.setCursor (13,0);
    lcd.print ("off");
    strcpy (OutputFlag, "off");
 }
  
 delay (2000);

}
//
// END OF FILE
//
