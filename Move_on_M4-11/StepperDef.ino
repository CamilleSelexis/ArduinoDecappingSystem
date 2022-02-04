//Nominal stepper speed
void stepper_std(){
  uint MPW = 30; 

  XSPoint->setMaxSpeed(10000);
  XSPoint->setAcceleration(7000);
  XSPoint->setMinPulseWidth(MPW);

  ZSPoint->setMaxSpeed(10000);
  ZSPoint->setAcceleration(7000);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(10000);
  CSPoint->setAcceleration(7000);
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(10000);
  MSPoint->setAcceleration(7000);
  MSPoint->setMinPulseWidth(MPW);

 }

 void stepper_screwspd() { //Set speed for screw/unscrew of Z and C
  uint MPW = 30;
//  long Zspeed = 3000;
//  long Zaccel = 3000;
//  float Cratio = stp1tour/mmtostep(pasLBW,Zgear,2);
//
//  ZSPoint->setMaxSpeed(Zspeed);
//  ZSPoint->setAcceleration(Zaccel);
//  ZSPoint->setMinPulseWidth(MPW);
//
//  CSPoint->setMaxSpeed(ceil(Zspeed*Cratio));
//  CSPoint->setAcceleration(ceil(Zaccel*Cratio));
//  CSPoint->setMinPulseWidth(MPW);
  RPC1.println("in stepperscrewspd");
  unsigned long Zmove = ceil(Zgear*motor_step_rot*micro_ratio); //in steps
  RPC1.println("in Zmove computed");
  unsigned long Cmove = ceil(stp1tour*0.8); //in steps
  RPC1.println("in Cmove computed");
  unsigned long Zspeed = ceil(Zmove/screw_time); //take 10 sec to unscrew the cap
  RPC1.println("in Zspeed computed");
  unsigned long Cspeed = ceil(Cmove/screw_time);
  RPC1.println("in Cspeed computed");
  ZSPoint->setMaxSpeed(Zspeed);
  ZSPoint->setAcceleration(Zspeed);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(Cspeed);
  CSPoint->setAcceleration(Cspeed);
  CSPoint->setMinPulseWidth(MPW);

 }

//Stepper speed used during the initialisation
 void stepper_init()  {
  uint MPW = 30;

  XSPoint->setMaxSpeed(2000);
  XSPoint->setAcceleration(6000);
  XSPoint->setMinPulseWidth(MPW);

  ZSPoint->setMaxSpeed(5000);
  ZSPoint->setAcceleration(6000);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(5000);
  CSPoint->setAcceleration(5000);
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(3000);
  MSPoint->setAcceleration(4000);
  MSPoint->setMinPulseWidth(MPW);
 }

 void slow_initZ() {
  ZSPoint->setMaxSpeed(2000);
  ZSPoint->setAcceleration(6000);
 }

void slow_initX() {
  XSPoint->setMaxSpeed(1500);
  XSPoint->setAcceleration(6000);
 }

void slow_initC() {
  CSPoint->setMaxSpeed(5000);
  CSPoint->setAcceleration(5000);
 }

void slow_initM() {
  MSPoint->setMaxSpeed(2000);
  MSPoint->setAcceleration(3000);
 }
