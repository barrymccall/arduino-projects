#include <ArduinoIcon.h>
#include <Arial14.h>
#include <ks0108.h>
#include <ks0108_Arduino.h>
#include <ks0108_Mega.h>
#include <ks0108_Panel.h>
#include <ks0108_Sanguino.h>
#include <SystemFont5x7.h>
#include "SystemFont5x7.h"   // system font
#include "ArduinoIcon.h"     // bitmap 

int batMonPin = 19;    // input pin for the divider
int buzzPin = 13;
int val = 0.0000;       // variable for the A/D value
float pinVoltage = 0.00000; // variable to hold the calculated voltage
float batteryVoltage = 0.00000;
float ratio = 3.133;  // Change this to match the MEASURED ration of the circuit
const int backLightPin = 13;

void printDouble( double val, byte precision){
  // prints val with number of decimal places determine by precision
  // precision is a number from 0 to 6 indicating the desired decimial places
  // example: printDouble( 3.1415, 2); // prints 3.14 (two decimal places)

  GLCD.PrintNumber( (long)val);  //prints the int part
  if( precision > 0) {
    GLCD.PutChar('.');
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision -1;
    while(precision--)
	 mult *=10;

    if(val >= 0)
	frac = (val - int(val)) * mult;
    else
	frac = (int(val)- val ) * mult;
    unsigned long frac1 = frac;
    while( frac1 /= 10 )
	padding--;
    while(  padding--)
	GLCD.PutChar('0');
    GLCD.PrintNumber(frac) ;
  }
}

void setup() {
  GLCD.Init(NON_INVERTED);       // initialise the library
  GLCD.ClearScreen();            // wipe it clean
  GLCD.SelectFont(System5x7);    // select fixed width system font 
  pinMode(backLightPin, OUTPUT);       // set 
  GLCD.GotoXY(0,0);
  GLCD.Puts ("Voltage=");
  }


void loop() {
val = analogRead(batMonPin);    // read the voltage on the divider  
  
  pinVoltage = val * 0.00488;       //  Calculate the voltage on the A/D pin
                                    //  A reading of 1 for the A/D = 0.0048mV
                                    //  if we multiply the A/D reading by 0.00488 then 
                                    //  we get the voltage on the pin.                                  
                                    
                                    
  
  batteryVoltage = pinVoltage * ratio;    //  Use the ratio calculated for the voltage divider
                                          //  to calculate the battery voltage
  GLCD.GotoXY(48,0);
  GLCD.Puts("     ");
  GLCD.GotoXY(48,0);
  printDouble(batteryVoltage,2);
  delay(1000);                  //  Slow it down

if (batteryVoltage < 9.3) {
  for (long i = 0; i < 2048 * 3; i++ )
  {
      // 1 / 2048Hz = 488uS, or 244uS high and 244uS low to create 50% duty cycle
      digitalWrite(buzzPin, HIGH);
      delayMicroseconds(244);
      digitalWrite(buzzPin, LOW);
      delayMicroseconds(244);
  }
}
}


