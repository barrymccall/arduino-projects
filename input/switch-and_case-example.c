#include <string.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

//Flash based string table. Necessary because otherwise the strings will take up all of our ram.
//Use either PrintLCDAt_P or PrintLCD_P to print these strings to the LCD.
prog_char string_0[] PROGMEM =   "  1. ENROLL USER    ";  //0
prog_char string_1[] PROGMEM =   "  2. EDIT USERS     ";  //1
prog_char string_2[] PROGMEM =   "  3. DELETE USERS   ";  //2
prog_char string_3[] PROGMEM =   "  4. DEL ALL USERS  ";  //3
prog_char string_4[] PROGMEM =   "  5. LIST USERS     ";  //4
prog_char string_5[] PROGMEM =   "  6. SET OPTIONS    ";  //5
prog_char string_6[] PROGMEM =   "	MAIN MENU     ";  //6

PGM_P PROGMEM StringTable[] = {
  string_0, string_1, string_2, string_3, string_4, string_5, string_6
};

//Global Variables for the Application

#define LCDrxpin  5//not used
#define LCDtxpin  6//we only transmit

#define KeyPin  1 //which analog pin is used for the 5 key pad

#define KeyLeft  10
#define KeyRight 20
#define KeyUp    30
#define KeyDown  40
#define KeySel   50
#define KeyInv   60   //invalid key value

SoftwareSerial LCDSerial = SoftwareSerial(LCDrxpin, LCDtxpin);

void setup()			  // run once, when the sketch starts
{
  byte Status;
  Status = LCDSetup(); //get the screen up and running
  if (Status == true) {
    delay(2000);
    mainMenu();
  }
}


void loop()			   // run over and over again
{
    
}


void mainMenu() {
  
  byte offset=0, loopcnt, whichkey, point=0;
  
  do {
    ClearLCD();
    PrintLCD_P(6); //"	Main Menu     "
    PrintLCDAt_P(offset, 0, 2);
    whichkey = PollKey();
    switch(whichkey) {
	 case KeyUp:
	    if (offset > 0) offset--;
	    break;
	 case KeyDown:
	    if (offset < 5) offset++;
	    break;
	 case KeySel:
	    switch (offset) {
		 case 0: //Enroll
		   // enrollUser();
		    break;
		 case 1: //Edit users
		    //editUser();
		    break;
		 case 2: //Delete Users
		    //deleteUser(false);
		    break;
		 case 3: //Delete all users
		    //deleteUser(true);
		    break;
		 case 4: //List Users
		    //listUsers();
		    break;
		 case 5: //Set options
		    //optionsMenu();
		    break;
	    }
	    break;
    }	  
  } while (1 == 1);
}


byte LCDSetup() {
  pinMode(LCDrxpin, INPUT);
  pinMode(LCDtxpin, OUTPUT);
  LCDSerial.begin(9600);
  delay(1000);  
  InitLCD();
  return true;
}

void InitLCD() {
  //make sure the initial parameters are right
  LCDSerial.print("?S0?f?G420");
   delay(120);
}

//Print a string at the specified location
//Does not try to clear the line or anything
void PrintLCDAt(char *inStr, char x, char y) {
   LCDSerial.print("?y");
   LCDSerial.print(y, DEC);
   delay(20);
   LCDSerial.print("?x");
   if (x < 10) LCDSerial.print("0");
   LCDSerial.print(x, DEC);
   delay(20);
   LCDSerial.print(inStr);
   delay(40);
}

//print a string found in program memory to the LCD at a certain location
//Basically this just sets the location and then calls PrintLCD_P to do the work
void PrintLCDAt_P(int which, char x, char y) {
   LCDSerial.print("?y");
   LCDSerial.print(y, DEC);
   delay(20);
   LCDSerial.print("?x");
   if (x < 10) LCDSerial.print("0");
   LCDSerial.print(x, DEC);
   delay(20);
   PrintLCD_P(which);  
}

//this is the gatekeeper function. The only one to really touch the strings in program memory. This
// keeps the need
//for a buffer string to only one. So, basically we're only using 21 bytes of memory total to store
// strings.
void PrintLCD_P(int which) {
   char buffer[21];
   strcpy_P(buffer, (char*)pgm_read_word(&(StringTable[which])));
   LCDSerial.print(buffer);
   delay(40);
}

//Clear the entire LCD
void ClearLCD() {
  LCDSerial.print("?f");
  delay(20);
}


//Scan the key pin and determine which key was pressed. Then return a code specifying each key
//There is a fuzzy range for each key. This is a central space for all key presses. That way it's
//easy to tweak these values if need be.
char KeyScan() {
   int which, which2, diff,retVal;
   which = analogRead(KeyPin);
   delay(20);
   which2 = analogRead(KeyPin);
   retVal = KeyInv;
   diff = abs(which - which2);
   if (diff < 12) {
	if (which > 2 && which < 36) retVal = KeyLeft;
	if (which > 58 && which < 100) retVal =  KeySel;
	if (which > 241 && which < 283) retVal =  KeyDown;
	if (which > 409 && which < 451) retVal =  KeyRight;
	if (which > 577 && which < 619) retVal =  KeyUp;
   }
   return retVal;
}

//repeatedly calls KeyScan until a valid key press occurs
char PollKey() {
  char Whichkey;
  
  do {
     Whichkey = KeyScan();
     delay(30);
  } while (Whichkey==KeyInv);
  delay(80);
  return Whichkey;
}