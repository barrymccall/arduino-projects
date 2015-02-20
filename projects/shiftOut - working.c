int serInputLatch = 6 ;  // The latch pin.  This is required in order for the shift register to take a copy of the current parallel input.
int serInputClock = 7 ;  // The clock pin.  
int serInputData  = 8 ;  // The serial in pin.  This will be read by digitalRead.  

int serOutputLatch = 4;
int serOutputClock = 5; 
int serOutputData = 3;

boolean registers[8];
boolean buttons[8];

byte relayMask = 0;
byte buttonMask = 0;

byte button1state;
byte button2state;
byte button3state;
byte button4state;
byte button5state;
byte button6state;
byte button7state;
byte button8state;

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

void loop(){
  buttonMask=shiftOut();
  if (buttonMask == 128){  
    Serial.print("Button 8.. yay\n");
    }
  else if (buttonMask==64){
    Serial.print(Button 7.. yay\n");
    }
  }
//button8Curstate = processButtonStates(1, button8static, button8Curstate, 1);
//processButtonStates(button2state, button2static, button2Curstate, 2);
    
    Serial.print(buttonMask, BIN);
    Serial.print("\n");
    delay(500);

    writeRelaysState();

}

// function for bit shifting out
// SN74HC165N specific
byte shiftOut(){
  byte byteVal = 0;
  digitalWrite(serInputLatch, LOW);
  delayMicroseconds(5);
  digitalWrite(serInputLatch, HIGH);
  
  for(int i = 7; i>=0; i--){
    byte bitVal = digitalRead(serInputData);
    //Serial.print(bitVal, BIN);
    byteVal |= (bitVal << i);
    digitalWrite(serInputClock, HIGH);
    delayMicroseconds(5);
    digitalWrite(serInputClock, LOW);
  }
  /* The following Serial.print lines are only
  // necessary for debuggins
  Serial.print("\n");
  Serial.print("post bitwise ops=");
  Serial.print(buttonsState, BIN);*/
  return (byteVal);
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

byte processButtonStates( byte buttonState, int buttonStatic, int buttonCurstate, int relayNum ){
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
  return(buttonCurstate);
}