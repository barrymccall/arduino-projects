int buttonUP   = 2;
int buttonDOWN = 4;
int pwmCtrl    = 9;
unsigned int speedValue = 80; // start speed value at 100;

boolean UPPressed = false;
boolean DOWNPressed = false;
  
void setup(){
  pinMode(pwmCtrl, OUTPUT);
  pinMode(buttonUP, INPUT);
  pinMode(buttonDOWN, INPUT);

  Serial.begin(9600);
}

void loop(){
  UPPressed = false;
  DOWNPressed = false;
  
  int buttonUPPress = digitalRead(buttonUP);
  int buttonDOWNPress = digitalRead(buttonDOWN);
  if (buttonUPPress == HIGH && speedValue < 255 && speedValue >= 0){
    speedValue += 1;
    UPPressed = true;
    }
  if (buttonDOWNPress == HIGH && speedValue >=1 && speedValue <= 255){
      speedValue -=1;
      DOWNPressed = true;  
  }
 
  analogWrite(pwmCtrl, speedValue);
  
  
  Serial.print(speedValue); /*Serial.print("; UPPressed:"); Serial.print(UPPressed); Serial.print("; DownPressed:"); Serial.print(DOWNPressed); */ Serial.print("\n");
  delay(250);
}
