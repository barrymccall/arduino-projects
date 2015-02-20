
// to use 
// printDouble({your float var},{number Of Decimal Places}
// printDouble(batteryVoltage,2);
// will use GLCD.PrintNumber and apply 2 decimal places.

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