/*
 
  This example code is in the public domain.
 
  ---------------------------------------------------------------------
 
  This program demonstrates button detection, LCD text/number printing,
  and LCD backlight control on the Freetronics LCD & Keypad Shield, connected to an Arduino board.
 
 
  Pins used by LCD & Keypad Shield:
 
    A0: Buttons, analog input from voltage ladder
    D4: LCD bit 4
    D5: LCD bit 5
    D6: LCD bit 6
    D7: LCD bit 7
    D8: LCD RS
    D9: LCD E
    D10: LCD Backlight (high = on, also has pullup high so default is on)
 
  ADC voltages for the 5 buttons on analog input pin A0:
 
    RIGHT:  0.00V :   0 @ 8bit ;   0 @ 10 bit
    UP:     0.71V :  36 @ 8bit ; 145 @ 10 bit
    DOWN:   1.61V :  82 @ 8bit ; 329 @ 10 bit
    LEFT:   2.47V : 126 @ 8bit ; 505 @ 10 bit
    SELECT: 3.62V : 185 @ 8bit ; 741 @ 10 bit
*/
/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/
#include <LiquidCrystal.h>   // include LCD library
#include <EEPROM.h>
/*--------------------------------------------------------------------------------------
  Defines
--------------------------------------------------------------------------------------*/
// Pins in use
#define BUTTON_ADC_PIN           A0  // A0 is the button ADC input
#define LCD_BACKLIGHT_PIN         10  // D10 controls LCD backlight
#define PWM_CTL                  11                                                            
// ADC readings expected for the 5 buttons on the ADC input
#define RIGHT_10BIT_ADC           0  // right
#define UP_10BIT_ADC            145  // up
#define DOWN_10BIT_ADC          329  // down
#define LEFT_10BIT_ADC          505  // left
#define SELECT_10BIT_ADC        741  // right
#define BUTTONHYSTERESIS         10  // hysteresis for valid button sensing window
//return values for ReadButtons()
#define BUTTON_NONE               0  // 
#define BUTTON_RIGHT              1  // 
#define BUTTON_UP                 2  // 
#define BUTTON_DOWN               3  // 
#define BUTTON_LEFT               4  // 
#define BUTTON_SELECT             5  // 
//some example macros with friendly labels for LCD backlight/pin control, tested and can be swapped into the example code as you like
#define LCD_BACKLIGHT_OFF()     digitalWrite( LCD_BACKLIGHT_PIN, LOW )
#define LCD_BACKLIGHT_ON()      digitalWrite( LCD_BACKLIGHT_PIN, HIGH )
#define LCD_BACKLIGHT(state)    { if( state ){digitalWrite( LCD_BACKLIGHT_PIN, HIGH );}else{digitalWrite( LCD_BACKLIGHT_PIN, LOW );} }
/*--------------------------------------------------------------------------------------
  Variables
--------------------------------------------------------------------------------------*/
byte buttonJustPressed  = false;         //this will be true after a ReadButtons() call if triggered
byte buttonJustReleased = false;         //this will be true after a ReadButtons() call if triggered
byte buttonWas          = BUTTON_NONE;   //used by ReadButtons() for detection of button events

// menu vars
byte mainMenu           = false;         //set if user enters menu, false when menu is exited

byte menuDispExit       = false;
byte atMenuExit         = false;

byte menuDispSetSpd     = false;
byte atMenuSetSpd       = false;
byte confMenuSetSpd     = false;

byte menuDispBacklight  = false;
byte atMenuBacklight    = false;
byte confMenuBacklight  = false;
// -------------------------------------------
// When the PWM signal is too low to start the motor cant spin the stir bar
// we init the hardware with this value at 100%, then REDUCE to the set speed 
// previously set in EEPROM

unsigned int memloc0 = 0;
unsigned int startupSpeedValue;

unsigned int memloc1 = 1;
byte backlight;
const char* backlightText = "";
unsigned int speedValue = 0;
byte lightVal;
/*--------------------------------------------------------------------------------------
  Init the LCD library with the LCD pins to be used
--------------------------------------------------------------------------------------*/
LiquidCrystal lcd( 3, 2, 7, 6, 5, 4 );   //Pins for the freetronics 16x2 LCD shield. LCD: ( RS, E, LCD-D4, LCD-D5, LCD-D6, LCD-D7 )
/*--------------------------------------------------------------------------------------
  setup()
  Called by the Arduino framework once, before the main loop begins
--------------------------------------------------------------------------------------*/
void setup()
{
   //button adc input
   pinMode( BUTTON_ADC_PIN, INPUT );         //ensure A0 is an input
   digitalWrite( BUTTON_ADC_PIN, LOW );      //ensure pullup is off on A0
   //lcd backlight control
   digitalWrite( LCD_BACKLIGHT_PIN, HIGH );  //backlight control pin D3 is high (on)
   pinMode( LCD_BACKLIGHT_PIN, OUTPUT );     //D3 is an output
   pinMode( PWM_CTL, OUTPUT);                //PWM_CTRL is an output to the mosfet
   // def lcd, begin( columns, rows )
   lcd.begin( 16, 2 );   
   // begin inital init to spin UP motor, and gradual down
   // to the set speedValue 
   
   startupSpeedValue = EEPROM.read(memloc0);
   if (startupSpeedValue >= 100 && startupSpeedValue <= 0){
     startupSpeedValue = 0;
   }
   
   backlight = EEPROM.read(memloc1);
    if (backlight == true){
      backlightText = "On";
      LCD_BACKLIGHT_ON();
    }
    else {
      backlightText = "Off";
      LCD_BACKLIGHT_OFF();
    }
    
   if (startupSpeedValue != 0){
      speedValue = SpinDown(startupSpeedValue);
   }


/*
   analogWrite(PWM_CTL, map(startupSpeedValue, 0, 100, 0, 255));
   delay(700);
   while (startupSpeedValue >= speedValue){ 
     lcd.setCursor( 0, 0 );   //top left
     lcd.print( "init spin down" );
     lcd.setCursor( 0, 1 );   //bottom left
     lcd.print( "Speed:" );
     lcd.setCursor( 7, 1 );
     lcd.print( startupSpeedValue );
     lcd.print("%   ");
     startupSpeedValue -= 1;
     analogWrite(PWM_CTL, map(startupSpeedValue, 0, 100, 0, 255));
     delay(10);
   } */
   lcd.setCursor( 0, 0 );   //top left
   lcd.clear();
   lcd.print( "Stir Plate" );
   lcd.setCursor( 0, 1 );   //bottom left
   delay(250);
   DisplaySpeed();
   
   Serial.begin(9600);
   analogWrite(PWM_CTL, speedValue);
   
   
}
/*--------------------------------------------------------------------------------------
  loop()
  Arduino main loop
--------------------------------------------------------------------------------------*/
void loop()
{
   byte button;
   byte timestamp;
 
   //get the latest button pressed, also the buttonJustPressed, buttonJustReleased flags
   button = ReadButtons();

   //show text label for the button pressed
   switch( button )
   {
      case BUTTON_NONE:
      {
         break;
      }
      case BUTTON_RIGHT:
      {
        if (confMenuBacklight == true){
          LCD_BACKLIGHT_OFF();
          backlight = false;
          lcd.clear();
          lcd.setCursor( 0, 0 );
          lcd.print( "Backlight:" );
          lcd.setCursor( 0, 1 );
          if (backlight == true){
            backlightText = "On";
          }
          else{ 
            backlightText = "Off";
          }
          lcd.print( backlightText );
          break;
        }
      }
      
      
      // UUUUUUPPPPPPPP
      case BUTTON_UP:
      {
         if (mainMenu == true){
          if (confMenuSetSpd == true && atMenuSetSpd == true){
            if (startupSpeedValue <= 100 && startupSpeedValue >= 0){
              startupSpeedValue += 1;
            }
           lcd.clear();
           lcd.setCursor( 0, 0);
           lcd.print( "Start speed:" );
           lcd.setCursor( 0, 1);
           lcd.print( startupSpeedValue );
           lcd.print("%");
           break;          
         }  

           if (atMenuSetSpd == true){
              atMenuSetSpd = false;
              lcd.clear();
              lcd.setCursor( 0, 0);
              lcd.print( "Menu:");
              lcd.setCursor( 0, 1);
              lcd.print( "Exit" );
              atMenuExit = true;
              break;
           }
           if (atMenuExit == true){
            atMenuExit = false;
            lcd.clear();
            lcd.setCursor( 0, 0);
            lcd.print( "Menu:" );
            lcd.setCursor( 0, 1);
            lcd.print( "Backlight Options" );
            atMenuBacklight = true;
            break;
          }
        
          if (atMenuBacklight = true){
            atMenuBacklight = false;
            lcd.clear();
            lcd.setCursor( 0, 0);
            lcd.print( "Menu:");
            lcd.setCursor( 0, 1);
            lcd.print( "Set Start Speed" );
            atMenuSetSpd = true;
            break;
          }

    }
      
         if (speedValue == 0){
             pinMode( PWM_CTL, OUTPUT);
             speedValue +=1; 
             DisplaySpeed();
             speedValue = SpinDown(speedValue);
             break;
           }
           
           if (speedValue < 100 && speedValue >= 1){
             speedValue +=1;
             DisplaySpeed();
           }
         break;
      }
      
      
      case BUTTON_DOWN:
      {
      // determine if we are in a menu, if so, which one?
      //  button actions occur at menu level (main menu, up/dn change menu)
      // 
         if (mainMenu == true){
         
        if (confMenuSetSpd == true && atMenuSetSpd == true){
          if (startupSpeedValue <= 100 && startupSpeedValue >= 0){
            startupSpeedValue -= 1;
          }
          lcd.clear();
          lcd.setCursor( 0, 0);
          lcd.print( "Start speed:" );
          lcd.setCursor( 0, 1);
          lcd.print( startupSpeedValue );
          lcd.print("%");
          break;          
        }
        
           if (atMenuSetSpd == true){
              atMenuSetSpd = false;
              lcd.clear();
              lcd.setCursor( 0, 0);
              lcd.print( "Menu:");
              lcd.setCursor( 0, 1);
              lcd.print( "Exit" );
              atMenuExit = true;
              break;
           }
           if (atMenuExit == true){
            atMenuExit = false;
            lcd.clear();
            lcd.setCursor( 0, 0);
            lcd.print( "Menu:" );
            lcd.setCursor( 0, 1);
            lcd.print( "Backlight Options" );
            atMenuBacklight = true;
            break;
          }
        
          if (atMenuBacklight = true){
            atMenuBacklight = false;
            lcd.clear();
            lcd.setCursor( 0, 0);
            lcd.print( "Menu:");
            lcd.setCursor( 0, 1);
            lcd.print( "Set Start Speed" );
            atMenuSetSpd = true;
            break;
          }
       
    }
                
           
        if (speedValue >= 1 && speedValue <= 100){
           speedValue -=1;
           DisplaySpeed();
           if (speedValue == 0){
             pinMode( PWM_CTL, LOW);
             pinMode( PWM_CTL, LOW);
             pinMode( PWM_CTL, LOW);
             } 
           break; 
       }

      }
      
      //  LEFTY
      //  LEFTY
      case BUTTON_LEFT:
      {
        if (confMenuBacklight == true){
          LCD_BACKLIGHT_ON();
          backlight = true;
          lcd.clear();
          lcd.setCursor( 0, 0 );
          lcd.print( "Backlight:" );
          lcd.setCursor( 0, 1 );
          if (backlight == true){
            backlightText = "On";
          }
          else {
            backlightText = "Off";
          }
          lcd.print( backlightText );
          break;
          
        }
     }
     
     
     
     
     
     
     case BUTTON_SELECT:
     {
        // if we were already in the menu, determine where
        // we are
        
        // if its on the exit menu
        if (atMenuExit == true){
          atMenuExit = false;
          mainMenu = false;
          atMenuBacklight = false;
          confMenuBacklight = false;
          atMenuSetSpd = false;
          confMenuSetSpd = false;
          DisplaySpeed();
          break;
        }
        
        if (atMenuSetSpd == true && confMenuSetSpd == false){
          menuDispSetSpd = false;
          confMenuSetSpd = true;
          lcd.clear();
          lcd.setCursor( 0, 0);
          lcd.print( "Start speed:" );
          lcd.setCursor( 0, 1);
          lcd.print( startupSpeedValue );
          lcd.print("%");
          break;
        }
        

        if (atMenuBacklight == true && confMenuBacklight == false){
          menuDispBacklight = false;
          confMenuBacklight = true;
          lcd.clear();
          lcd.setCursor( 0, 0 );
          lcd.print( "Backlight:" );
          lcd.setCursor( 0, 1 );
          lcd.print( backlightText );
          break;
        }
        
        if (mainMenu == false){
          mainMenu = true;
          lcd.clear();
          lcd.setCursor( 0, 0 );
          lcd.print( "Menu:" );
          lcd.setCursor( 0, 1 );
          lcd.print( "UP/DN for options");
          break;
        }
        
        if (confMenuBacklight == true && atMenuBacklight == true){
          EEPROM.write(memloc1, backlight);
          lcd.clear();
          lcd.setCursor( 0, 0 );
          lcd.print( "Option saved!" );
          delay(1750);
          confMenuBacklight = false;
          lcd.clear();
          lcd.setCursor( 0, 0);
          lcd.print( "Menu:" );
          lcd.setCursor( 0, 1);
          lcd.print( "Backlight Options" );
          break;
        }
        if (atMenuSetSpd == true && confMenuSetSpd == true){
          EEPROM.write(memloc0, startupSpeedValue);
          lcd.clear();
          lcd.setCursor( 0, 0 );
          lcd.print( "Option saved!" );
          delay(950);
          confMenuSetSpd = false;
          lcd.clear();
          lcd.setCursor( 0, 0);
          lcd.print( "Menu:" );
          lcd.setCursor( 0, 1);
          lcd.print( "Set Start Speed" );
          break;
        }
      }
      default:
     {
        break;
     }

   }
  // re-map the speedValue to 0-255 for analog write 
  if (speedValue >= 1){
    analogWrite(PWM_CTL, map(speedValue, 0, 100, 40, 255));
  }
/*
   // print the number of seconds since reset (two digits only)
   timestamp = ( (millis() / 1000) % 100 );   //"% 100" is the remainder of a divide-by-100, which keeps the value as 0-99 even as the result goes over 100
   lcd.setCursor( 14, 1 );
   if( timestamp <= 9 )
      lcd.print( " " );   //quick trick to right-justify this 2 digit value when it's a single digit
   lcd.print( timestamp, DEC );
*/  
   //debug/test display of the adc reading for the button input voltage pin.
   //clear the buttonJustPressed or buttonJustReleased flags, they've already done their job now.
   if( buttonJustPressed )
      buttonJustPressed = false;
   if( buttonJustReleased )
      buttonJustReleased = false;
      
//  Serial.print(speedValue); Serial.print("  ");
//  Serial.print("ADC Val:"); Serial.print( analogRead( BUTTON_ADC_PIN ) ); Serial.print("\n");
//  Serial.print("speed_val:"); Serial.print(speedValue); Serial.print("\n");
//Serial.print("startupSpeedValue= "); Serial.print(startupSpeedValue);  Serial.print("\n");
//Serial.print("confMenuSetSpd= "); Serial.print(confMenuSetSpd); Serial.print("\n");
//  Serial.print(lightVal); Serial.print("\n");
  delay(250);

}





void DisplaySpeed(){
  if (speedValue == 0){
     lcd.clear();
     lcd.setCursor( 7, 1 );
     lcd.print( "            " );
     lcd.setCursor( 0, 1 );   //bottom left
     lcd.print( "Speed:" );
     lcd.setCursor( 7, 1 );
     lcd.print( "Off" );
  }
 if (speedValue >= 1){        
     lcd.clear();
     lcd.setCursor( 7, 1 );
     lcd.print( "            " );
     lcd.setCursor( 0, 1 );   //bottom left
     lcd.print( "Speed:" );
     lcd.setCursor( 7, 1 );
     lcd.print( speedValue );
     lcd.print("%");
 }
}


// PWM is fickle with a heavy motor. We have to HIT the motor
// with juice on the PWM output to get it spinning 
// then we spin down fast
unsigned int SpinDown(int spinDownTo){
  int startVal = 100;   // start at 100%
  analogWrite(PWM_CTL, map(startVal, 0, 100, 0, 255));  // hit the PWM_CTL with full power
  delay(1000);   // delay
  // now spin down and set speedValue
   while (startVal > spinDownTo){ 
     delay(75);
     startVal -= 1;
     analogWrite(PWM_CTL, map(spinDownTo, 0, 100, 40, 255));
     return spinDownTo;
   }
}



/*--------------------------------------------------------------------------------------
  ReadButtons()
  Detect the button pressed and return the value
  Uses global values buttonWas, buttonJustPressed, buttonJustReleased.
--------------------------------------------------------------------------------------*/
byte ReadButtons()
{
   unsigned int buttonVoltage;
   byte button = BUTTON_NONE;   // return no button pressed if the below checks don't write to btn
 
   //read the button ADC pin voltage
   buttonVoltage = analogRead( BUTTON_ADC_PIN );
   //sense if the voltage falls within valid voltage windows
   if( buttonVoltage < ( RIGHT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_RIGHT;
   }
   else if(   buttonVoltage >= ( UP_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( UP_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_UP;
   }
   else if(   buttonVoltage >= ( DOWN_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( DOWN_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_DOWN;
   }
   else if(   buttonVoltage >= ( LEFT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( LEFT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_LEFT;
   }
   else if(   buttonVoltage >= ( SELECT_10BIT_ADC - BUTTONHYSTERESIS )
           && buttonVoltage <= ( SELECT_10BIT_ADC + BUTTONHYSTERESIS ) )
   {
      button = BUTTON_SELECT;
   }
   //handle button flags for just pressed and just released events
   if( ( buttonWas == BUTTON_NONE ) && ( button != BUTTON_NONE ) )
   {
      //the button was just pressed, set buttonJustPressed, this can optionally be used to trigger a once-off action for a button press event
      //it's the duty of the receiver to clear these flags if it wants to detect a new button change event
      buttonJustPressed  = true;
      buttonJustReleased = false;
   }
   if( ( buttonWas != BUTTON_NONE ) && ( button == BUTTON_NONE ) )
   {
      buttonJustPressed  = false;
      buttonJustReleased = true;
   }
 
   //save the latest button value, for change event detection next time round
   buttonWas = button;
 
   return( button );
}

