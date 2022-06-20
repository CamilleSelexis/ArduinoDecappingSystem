//Nominal stepper speed
void stepper_std(){

  ZSPoint->setMaxSpeed(Zspeed*micro_ratio); //9000
  ZSPoint->setAcceleration(Zacc*micro_ratio);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(Cspeed*micro_ratio); //9000
  CSPoint->setAcceleration(Cacc*micro_ratio);
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(Mspeed*micro_ratio); //10000
  MSPoint->setAcceleration(Macc*micro_ratio);
  MSPoint->setMinPulseWidth(MPW);

 }

 void stepper_screwspd() { //Set speed for screw/unscrew of Z and C
// Z must move by 8 mm = 4 full turn
// while C does 0.8 turn
// Can increase Zmove a bit more  
  int screw_time = 4; 
  unsigned long Zmove = ceil(Zgear*motor_step_rot*micro_ratio*4.1); //in steps
  unsigned long Cmove = ceil(stp1tour*0.8); //in steps
  unsigned long Zscrspeed = ceil(Zmove/(ScrewSpeed/micro_ratio)); //take 10 sec to unscrew the cap
  unsigned long Cscrspeed = ceil(Cmove/(ScrewSpeed/micro_ratio));
  RPC.println(ScrewSpeed);
  RPC.println(Zscrspeed);
  ZSPoint->setMaxSpeed(Zscrspeed);
  ZSPoint->setAcceleration(Zscrspeed);
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(Cscrspeed);
  CSPoint->setAcceleration(Cscrspeed);
  CSPoint->setMinPulseWidth(MPW);

 }

//Stepper speed used during the initialisation
 void stepper_init()  {

  ZSPoint->setMaxSpeed(ceil(Zspeed*micro_ratio*0.5));
  ZSPoint->setAcceleration(ceil(Zacc*micro_ratio*0.5));
  ZSPoint->setMinPulseWidth(MPW);

  CSPoint->setMaxSpeed(ceil(Cspeed*micro_ratio*0.5));
  CSPoint->setAcceleration(ceil(Cacc*micro_ratio*0.5));
  CSPoint->setMinPulseWidth(MPW);

  MSPoint->setMaxSpeed(ceil(Mspeed*micro_ratio*0.5));
  MSPoint->setAcceleration(ceil(Macc*micro_ratio*0.5));
  MSPoint->setMinPulseWidth(MPW);
 }

 void slow_initZ() {
  ZSPoint->setMaxSpeed(ceil(Zspeed*micro_ratio*0.5));
  ZSPoint->setAcceleration(ceil(Zacc*micro_ratio*0.5));
  ZSPoint->setMinPulseWidth(MPW);
 }

void slow_initC() {
  CSPoint->setMaxSpeed(ceil(Cspeed*micro_ratio*0.5));
  CSPoint->setAcceleration(ceil(Cacc*micro_ratio*0.5));
  CSPoint->setMinPulseWidth(MPW);
 }

void slow_initM() {
  MSPoint->setMaxSpeed(ceil(Mspeed*micro_ratio*0.5));
  MSPoint->setAcceleration(ceil(Macc*micro_ratio*0.5));
  MSPoint->setMinPulseWidth(MPW);
 }
