//Interuptions
//All function with the rising edge, set the position to 0 and write a move to 1000
void pin_init() {
  pinMode(pin_photo,INPUT);           //Photo-detector - C axis zero
  pinMode(pin_contact,INPUT);         //Contacteur - M axis zero
  pinMode(pin_BZ,INPUT_PULLDOWN);     //Z-axis zero
  attachInterrupt(digitalPinToInterrupt(pin_BZ), reset_Z, FALLING);
}

void pin_reinit(){
  pinMode(D2,OUTPUT); //Step M
  pinMode(D3,OUTPUT); //Step Z
  pinMode(D4,OUTPUT); //Step C
  pinMode(D13,OUTPUT); //Direction X,M
  pinMode(D14,OUTPUT); //Direction Z,C
}

void reset_Z()  {
  ZSPoint->stop();
}

bool checkSens(uint thres) { //Check if the optical barrier is in front or not of the detector
  uint n =10;
  uint res = 0;
  for(int i = 0; i<n; i++){
    res += analogRead(pin_photo);
    delayMicroseconds(5);
  }
  return res > n*thres;
}

void deBuff() {
//  for(uint8_t i = 0; i<20; i++){
//    runALL;
//    delayMicroseconds(10);
//  }
}
