//Nominal stepper speed
void stepper_std(){

  XSPoint->setMaxSpeed(10000); //10000
  XSPoint->setAcceleration(7000);
  XSPoint->setMinPulseWidth(MPW);

  ZSPoint->setMaxSpeed(9000); //9000
  ZSPoint->setAcceleration(7000);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(9000); //9000
  CSPoint->setAcceleration(6000);
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(10000); //10000
  MSPoint->setAcceleration(7000);
  MSPoint->setMinPulseWidth(MPW);

 }

 void stepper_screwspd() { //Set speed for screw/unscrew of Z and C
// Z must move by 8 mm = 4 full turn
// while C does 0.8 turn
// Can increase Zmove a bit more
  unsigned long Zmove = ceil(Zgear*motor_step_rot*micro_ratio*4.1); //in steps
  unsigned long Cmove = ceil(stp1tour*0.8); //in steps
  unsigned long Zspeed = ceil(Zmove/screw_time); //take 10 sec to unscrew the cap
  unsigned long Cspeed = ceil(Cmove/screw_time);
  ZSPoint->setMaxSpeed(Zspeed);
  ZSPoint->setAcceleration(Zspeed);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(Cspeed);
  CSPoint->setAcceleration(Cspeed);
  CSPoint->setMinPulseWidth(MPW);

 }

//Stepper speed used during the initialisation
 void stepper_init()  {

  XSPoint->setMaxSpeed(2200);
  XSPoint->setAcceleration(6000);
  XSPoint->setMinPulseWidth(MPW);

  ZSPoint->setMaxSpeed(5500);
  ZSPoint->setAcceleration(6000);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(5500);
  CSPoint->setAcceleration(5000);
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(3300);
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
