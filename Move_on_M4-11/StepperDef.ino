//Nominal stepper speed
void stepper_std(){

  ZSPoint->setMaxSpeed(4000*micro_ratio); //9000
  ZSPoint->setAcceleration(2000*micro_ratio);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(6000*micro_ratio); //9000
  CSPoint->setAcceleration(3500*micro_ratio);
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(3000*micro_ratio); //10000
  MSPoint->setAcceleration(2000*micro_ratio);
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

  ZSPoint->setMaxSpeed(2750*micro_ratio);
  ZSPoint->setAcceleration(3000*micro_ratio);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(2750*micro_ratio);
  CSPoint->setAcceleration(2500*micro_ratio);
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(1650*micro_ratio);
  MSPoint->setAcceleration(2000*micro_ratio);
  MSPoint->setMinPulseWidth(MPW);
 }

 void slow_initZ() {
  ZSPoint->setMaxSpeed(1300*micro_ratio);
  ZSPoint->setAcceleration(3000*micro_ratio);
 }

void slow_initC() {
  CSPoint->setMaxSpeed(3000*micro_ratio);
  CSPoint->setAcceleration(2500*micro_ratio);
 }

void slow_initM() {
  MSPoint->setMaxSpeed(1300*micro_ratio);
  MSPoint->setAcceleration(1500*micro_ratio);
 }
