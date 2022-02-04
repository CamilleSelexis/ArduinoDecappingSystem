void pin_init(){
  pinMode(LEDG, OUTPUT);       //Setting up the led
  
  pinMode(D9, OUTPUT);          //Motors X,Z,C,M en/dis-able
  digitalWrite(D9,HIGH);
  pinMode(A4, OUTPUT);          //Motors Z,C,M en/dis-able
  digitalWrite(A4,HIGH);
  pinMode(A6, OUTPUT);          //Motors Z,C,M en/dis-able
  digitalWrite(A6,HIGH);
  pinMode(D7,OUTPUT);           //Steady-state relay control
  digitalWrite(D7,LOW);
  pinMode(A0,INPUT);pinMode(A2,INPUT);pinMode(D6,INPUT_PULLDOWN);pinMode(D0,INPUT_PULLDOWN); //Detector C,M,Z,X
  attachInterrupt(digitalPinToInterrupt(D6), interrupt_X, FALLING);
  attachInterrupt(digitalPinToInterrupt(D0), interrupt_Z, FALLING);
  pinMode(D13,OUTPUT);pinMode(D14,OUTPUT);
  pinMode(D5,OUTPUT);pinMode(D4,OUTPUT); // Step X / Step C
  pinMode(D3,OUTPUT);pinMode(D2,OUTPUT); //Step Z / Step M
}

void interrupt_X() {
  flagX = 1;
  Xpointer->stop();}
void interrupt_Z() {
  flagZ = 1;
  Zpointer->stop();}
