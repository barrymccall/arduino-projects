int serInputLatch = 6 ;  // The latch pin.  This is required in order for the shift register to take a copy of the current parallel input.
int serInputClock = 7 ;  // The clock pin.  
int serInputData  = 8 ;  // The serial in pin.  This will be read by digitalRead.  

int serOutputLatch = 4;
int serOutputClock = 5; 
int serOutputData = 3;

boolean registers[8];

byte relay1state = 0;
byte relay2state = 0;
byte relay3state = 0;
byte relay4state = 0;
byte relay5state = 0;
byte relay6state = 0;
byte relay7state = 0;
byte relay8state = 0;

byte button1state = 0 ;
byte button2state = 0 ;
byte button3state = 0 ;
byte button4state = 0 ;
byte button5state = 0 ;
byte button6state = 0 ;
byte button7state = 0 ;
byte button8state = 0 ;

byte mySerData = 0 ;

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
}

void loop()
{
  // Latch the shift register's state.
  // Do 8 reads of the serialInputData serialInputData to obtain the shift register's state.
  // Display the state of each button.
  
  Serial.print("Start Read  ");
  readButtons();  // Get the data from the shift register.
  Serial.print(button1state, DEC); 
  Serial.print(button2state, DEC); 
  Serial.print(button3state, DEC); 
  Serial.print(button4state, DEC);
  Serial.print(button5state, DEC); 
  Serial.print(button6state, DEC); 
  Serial.print(button7state, DEC); 
  Serial.print(button8state, DEC);
  Serial.println("  End  Read");
  digitalWrite(serOutputLatch, LOW); delayMicroseconds(5); digitalWrite(serOutputLatch, HIGH);
  digitalWrite(serOutputData, mySerData); digitalWrite(serInputClock, HIGH); delayMicroseconds(5); digitalWrite(serOutputClock, LOW); 
  //delay(10);
  //setRegisterPin(1, HIGH);
  //writeRegisters();
}

void readButtons()
{
  digitalWrite(serInputLatch, LOW);  delayMicroseconds(5);  digitalWrite(serInputLatch, HIGH);
  button1state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button2state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);  
  button3state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button4state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button5state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button6state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button7state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
  button8state = digitalRead(serInputData);   digitalWrite(serInputClock, HIGH);   delayMicroseconds(5);   digitalWrite(serInputClock, LOW);
}

//void triggerRelay(){
//  relay1state = digitalWrite(serialOutData)
//  digitalWrite(serOutputLatch, LOW); delayMicroseconds(5); digitalWrite(serOutputLatch, HIGH);
//}

void clearRegisters(){
  for(int i = 8; i >  0; i--){
     registers[i] = LOW;
  }
}

void setRegisterPin(int index, int value){
  registers[index] = value;
}

void writeRegisters(){

  digitalWrite(serOutputClock, LOW);

  for(int i = 8; i >  0; i--){
    digitalWrite(serOutputClock, LOW);

    int val = registers[i];

    digitalWrite(serOutputData, val);
    digitalWrite(serOutputClock, HIGH);

  }
  digitalWrite(serOutputClock, HIGH);

}

