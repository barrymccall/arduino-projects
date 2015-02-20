// Parallel-In Shift Register Test
// Author:  Nickolas Andersen
// Email:  nicksen782 at yahoo dot com
// Original Creation Date:  02-22-2011
//
// What it does:
// Reads 8 push-button inputs via a SN74HC165 parallel-in/serial-out shift register.
// This will require only 3 GP I/O pins instead of 8 for the buttons.
// The sketch meant to very simple and simpily puts sequential digitalReads into the corresponding button state variables.
// The output is shown via the serial console. 

int pLatch = 6 ;  // The latch pin.  This is required in order for the shift register to take a copy of the current parallel input.
int pClock = 7 ;  // The clock pin.  
int pIN    = 8 ;  // The serial in pin.  This will be read by digitalRead.  

byte button1state = 0 ;
byte button2state = 0 ;
byte button3state = 0 ;
byte button4state = 0 ;
byte button5state = 0 ;
byte button6state = 0 ;
byte button7state = 0 ;
byte button8state = 0 ;

void setup()
{
  Serial.begin(115200);
  pinMode (pLatch, OUTPUT);
  pinMode (pClock, OUTPUT);
  pinMode (pIN, INPUT);

  digitalWrite(pClock, LOW);  // Since it is LOW to HIGH triggered, set it to LOW.
  digitalWrite(pLatch, HIGH); // Don't latch the parallel data yet, but give this pin an initial state.
}

void loop()
{
  // Latch the shift register's state.
  // Do 8 reads of the pIN pin to obtain the shift register's state.
  // Display the state of each button.
  
  Serial.print("Start Read  ");
  readit();  // Get the data from the shift register.
  Serial.print(button1state, DEC); 
  Serial.print(button2state, DEC); 
  Serial.print(button3state, DEC); 
  Serial.print(button4state, DEC);
  Serial.print(button5state, DEC); 
  Serial.print(button6state, DEC); 
  Serial.print(button7state, DEC); 
  Serial.print(button8state, DEC);
  Serial.println("  End  Read");
  delay(10);
}

void readit()
{
  digitalWrite(pLatch, LOW);  delayMicroseconds(5);  digitalWrite(pLatch, HIGH);
  button1state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);
  button2state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);  
  button3state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);
  button4state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);
  button5state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);
  button6state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);
  button7state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);
  button8state = digitalRead(pIN);   digitalWrite(pClock, HIGH);   delayMicroseconds(5);   digitalWrite(pClock, LOW);
}

