 void stepper_init(){

  int MPW = 30; 
  Xpointer->setMaxSpeed(10000);
  Xpointer->setAcceleration(10000);
  Xpointer->setMinPulseWidth(MPW);

  Zpointer->setMaxSpeed(16000);
  Zpointer->setAcceleration(16000);
  Zpointer->setMinPulseWidth(MPW);

  Cpointer->setMaxSpeed(12000);
  Cpointer->setAcceleration(12000);
  Cpointer->setMinPulseWidth(MPW);

  Mpointer->setMaxSpeed(12000);
  Mpointer->setAcceleration(12000);
  Mpointer->setMinPulseWidth(MPW);
 }
