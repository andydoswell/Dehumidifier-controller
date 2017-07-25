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
boolean Dew = false; // This is the dew flag. Off as default.

void setup()
{

  lcd.begin(16, 2); // defines the LCD as a 16 x 2
  pinMode (rlyPin, OUTPUT); // sets our relay pin
  digitalWrite (rlyPin, HIGH); // sets the relay off for default condition.

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

// Dew detect loop. If the dewTrip point is reached, start the timer running and set the Dew flag
  if ( temp <= dewTrip ) {
    Dew = true;
    Timer = 1;       
  } 
  else {
    Dew = false;
}

  
  if (Timer >= TimerLength and Dew == false) { // If the timer has expired and there's no dew, switch the dehumidifier off.
    Timer = 0;
    digitalWrite (rlyPin, HIGH);
   
  }

  if (Timer !=0) {                // If the timer is running, switch the dehumidifier on , and write On to the lcd.
    digitalWrite (rlyPin, LOW);
    lcd.setCursor (13,0);
    lcd.print ("On");
    Timer++;
  }
  
 else {
   lcd.setCursor (13,0);
    lcd.print ("off");
 }
  
// Watchdog loop - blinks a curson in the bottom right of the LCD, just to let us know the thing is still running
  if ( WatchDog == 0 ) {
    lcd.setCursor (15, 1);
    lcd.print (" ");
    WatchDog++ ;
  }
  else {
    
    WatchDog-- ;
    lcd.setCursor (15, 1);
    lcd.print ((char)0xFF);
  }
      


  delay(2000); // we can only get data from the sensor once every 2 seconds.
}
//
// END OF FILE
//
