void pin_init(){
 pinMode(LEDB, OUTPUT);       //Setting up the led
 pinMode(LEDG,OUTPUT);
 pinMode(D9, OUTPUT);          //Motors X,Z,C,M en/dis-able
 digitalWrite(D9,LOW);        
 pinMode(pin_crydom,OUTPUT);           //Steady-state relay control
 digitalWrite(pin_crydom,LOW);
 pinMode(A0,INPUT);      //Photo-detector - C axis zero (on M4)
 pinMode(A2,INPUT);      //Contacteur - M axis zero (on M4)
 pinMode(D0,INPUT);      //Baumer Z axis zero (on M4)
 pinMode(D6,INPUT);      //Baumer X axis zero (on M4)
}

void M4working(bool working){
  *Pworking = working;
}
