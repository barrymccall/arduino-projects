int serInputLatch = 6 ;  // The latch pin.  This is required in order for the shift register to take a copy of the current parallel input.
int serInputClock = 7 ;  // The clock pin.  
int serInputData  = 8 ;  // The serial in pin.  This will be read by digitalRead.  


int serOutputLatch = 4;
int serOutputClock = 5; 
int serOutputData = 3;


boolean registers[8];
boolean buttons[8];

byte relay1state = 1;
byte relay2state = 0;
byte relay3state = 0;
byte relay4state = 1;
byte relay5state = 0;
byte relay6state = 0;
byte relay7state = 0;
byte relay8state = 0;

byte buttonsState;
byte temp;

byte button1Curstate;
byte button2Curstate;
byte button3Curstate;
byte button4Curstate;
byte button5Curstate;
byte button6Curstate;
byte button7Curstate;
byte button8Curstate;



byte button1static;
byte button2static;
byte button3static;
byte button4static;
byte button5static;
byte button6static;
byte button7static;
byte button8static;

void setup()
{
  Serial.begin(115200);
  pinMode (serInputLatch, OUTPUT);
  pinMode (serInputClock, OUTPUT);
  pinMode (serInputData, INPUT);
  
  pinMode (serOutputLatch, OUTPUT);
  pinMode (serOutputClock, OUTPUT);
  pinMode (serOutputData, OUTPUT);
  
  digitalWrite(serInputClock, LOW);  // Since it is LOW to HIGH triggered, set it to LOW.
  digitalWrite(serInputLatch, HIGH); // Don't latch the parallel data yet, but give this pin an initial state.
  digitalWrite(serOutputClock, LOW);

  // we'll set button1 static and button2 dynamic 
  // for testing
  button1static = 1;
  button2static = 1;
}

void loop()
{
  digitalWrite(serInputLatch, LOW);
  delayMicroseconds(20);
  digitalWrite(serInputLatch, HIGH);
  temp = readButtons();
// processButtonStates(button1state, button1static, button1Curstate, 1);
// processButtonStates(button2state, button2static, button2Curstate, 2);
  
  Serial.print(temp, BIN);
  Serial.print("\n");
  delay(500);

// debugging - simple output to switch between a dynamic and static setting for a button/accessory

    
  writeRelaysState();

}

byte readButtons()
{
  /*digitalWrite(serInputLatch, LOW);  delayMicroseconds(5);  digitalWrite(serInputLatch, HIGH);
  button1state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button2state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);  
  button3state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button4state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button5state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button6state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button7state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button8state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  */

  
  for(int i = 7; i>=0; i--){
    buttonsState = digitalRead(serInputData);
    buttonsState = buttonsState << i;
    digitalWrite(serInputClock, HIGH);
    delayMicroseconds(.2);
    digitalWrite(serInputClock, LOW);
  }
  
}

void allRelaysOff(){
  for(int i = 8; i >  0; i--){
     registers[i] = LOW;
  }
}

void setRelay(int index, int value){
  registers[index] = value;
}

void writeRelaysState(){
  digitalWrite(serOutputLatch, LOW); 
  delayMicroseconds(5); 
  digitalWrite(serOutputLatch, HIGH);
  digitalWrite(serOutputClock, LOW);

  for(int i = 8; i >  0; i--){
    digitalWrite(serOutputClock, LOW);
    digitalWrite(serOutputData, registers[i]);
    digitalWrite(serOutputClock, HIGH);

  }
  digitalWrite(serOutputClock, HIGH);

}

void processButtonStates( int buttonState, int buttonStatic, int buttonCurstate, int relayNum ){
	if (buttonState == 1){
      // see if I'm set to be a static mode button
		if (buttonStatic == 1){
			// im a static mode button... 
			// am I on or off right now?
			if (buttonCurstate == 1){
				// I was on... turn the relay off
				setRelay(relayNum, LOW);   
				// tell the program I'm off
				buttonCurstate = 0;
				// chill a sec.. human hands aren't that fast
				delay(250);
			}
			else if (buttonCurstate == 0){
				// i was off.. turn the relay on
				setRelay(relayNum, HIGH);
				// tell the program I'm on
				buttonCurstate = 1;
				delay(250); 
			}
		}
		else 
			setRelay(relayNum, HIGH);
	}
	else 
		if (buttonStatic == 0){
		setRelay(relayNum, LOW);
	}
}

		