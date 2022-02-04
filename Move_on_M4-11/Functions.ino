//----------Movements------------//

//Ref all home Initialisation
void Ref()  {
  unsigned long setup_start_time = millis();
  noInterrupts(); //Disable the interrupts during init as we want to touch the limits
  stopALL;
  deBuff();
  stepper_init(); //Init steppers with init speeds
  RPC1.println("Starting Ref");
  if(!digitalRead(pin_BZ)){ //Get away from Z baumer
    RPC1.println("On the Z baumer, going away");
    ZSPoint->move(10000);
    while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}
  }
  if(!digitalRead(pin_BX)){ //Get away from X baumer
    RPC1.println("On the X baumer, going away");
    XSPoint->move(5000);
    while(abs(XSPoint->distanceToGo())>0){stepperX.run();}
  }
  MSPoint->move(7500);        //Let go of a potential lid
  CSPoint->move(-5000);
  while(abs(MSPoint->distanceToGo())>0||abs(CSPoint->distanceToGo())>0){runMC;}
  deBuff();
  //----------------------------Set Z 0 Position-----------------------
  refZ();
  //----------------------------------Set X 0 Position -----------------
  refX();
  //----------------------------Set the origin for the claws
  refM();
  //-----------------------------Set the origin for the rotation
  interrupts();
  refC();
  while(XSPoint->isRunning()||ZSPoint->isRunning()||MSPoint->isRunning()){
    runALL;
  }
  stopALL;
  stepper_std();
  //Can be more compact
  MSPoint->moveTo(Mzero_offset);
  ZSPoint->moveTo(Zzero_offset);
  XSPoint->moveTo(Xzero_offset); 
  runXZMC_toTargets(Xzero_offset,Zzero_offset,Mzero_offset,0);
  RPC1.println("done, I took " + String(millis()-setup_start_time) + " [ms] to initialize");
}

void refZ() {
  ZSPoint->move(-800000);    //Go up and search limit
  RPC1.println("Going Up to find my limits");
  while(digitalRead(pin_BZ)){stepperZ.run();}//interrupt stop
  deBuff();
  ZSPoint->move(6000);        // leave the switch
  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}
  deBuff();
  slow_initZ(); //slow the stepperZ
  ZSPoint->move(-10000);       //Go back a second time
  while(digitalRead(pin_BZ)){stepperZ.run();}
  deBuff();
  ZSPoint->setCurrentPosition(0);
  ZSPoint->move(3000);    //Get away from the switch
  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}
  RPC1.println("Ref Z is done");
}
void refX() {
  deBuff();
  XSPoint->move(-2000000);    //Go far from the platform, search limit
  RPC1.println("Going away to find my limits");
  while(digitalRead(pin_BX)){stepperX.run();}
  deBuff();
  XSPoint->move(3000); //Get away from the Baumer X
  while(abs(XSPoint->distanceToGo())>0){stepperX.run();}
  deBuff();
  slow_initX();
  XSPoint->move(-3500);    //Go back a second time
  while(digitalRead(pin_BX)){stepperX.run();}
  deBuff();
  XSPoint->setCurrentPosition(0);
  XSPoint->move(3000);
  while(abs(XSPoint->distanceToGo())>0){stepperX.run();}
  deBuff();
  RPC1.println("Ref X is done");
}
void refM() {
  MSPoint->move(-2000000);  //Go to first contact closure
  while(!digitalRead(pin_contact)){stepperM.run();}
  deBuff();
  MSPoint->move(8000);
  while(abs(MSPoint->distanceToGo())>0){stepperM.run();}
  deBuff();
  slow_initM();
  MSPoint->move(-10000);    //Go back a second time
  while(!digitalRead(pin_contact)){stepperM.run();}  //Interupt contain the stp_Z move to 2000
  MSPoint->setCurrentPosition(0);
  MSPoint->move(3000); //Go to an absolute position
  while(abs(MSPoint->distanceToGo())>0){stepperM.run();}
  RPC1.println("Ref M is done");
}
void refC() {
  slow_initC();
  bool barrier= false;
  bool done_C = false;
  if(analogRead(pin_photo)>950){
    barrier = false;
    RPC1.println("Not on the barrier, going CCW");
    CSPoint->move(-50000);
  }
  else{
    barrier = true;
    RPC1.println("In front of the barrier, going CW");
    CSPoint->move(100000);
  }
  if(barrier){
    while(!done_C){
      CSPoint->run();
      if(analogRead(A0)>950){
        done_C = true;
        CSPoint->stop();
        CSPoint->setCurrentPosition(0);
        RPC1.println("Rotation axe aligned");
      }
    }
  }
  else{ //Turn counter clockwise
    while(!done_C){
      //RPC1.println("Going CCW");
      CSPoint->run();
      if(analogRead(A0)< 300){
        done_C = true;
        CSPoint->stop();
        CSPoint->setCurrentPosition(0);
        RPC1.println("Rotation axe aligned");
      }
    }
  }
}
//Get the bottle an bring it back under the arm
void Get_flask(){

  XSPoint->moveTo(X_pos);
  ZSPoint->moveTo(Z_inter);
//  while(abs(XSPoint->distanceToGo())>0){runXZ;}
//  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}
  runXZMC_toTargets(X_pos,Z_inter,0,0);
  deBuff();
  RPC1.println("Closed the X axis to get the flask");
}

//Tightening the claws according to the info from video
//does not tighten at all lol
//Align the claws with the cap
void Align(long C_pos) {
  RPC1.println("Align the claws with the cap");
  CSPoint->moveTo(C_pos);
  while(abs(CSPoint->distanceToGo())>0){stepperC.run();}
  deBuff();
}

void Getdown(){ //Get down on the Cap and once arrived, tighten the claws
    RPC1.println("Going down, and then tightening the claws");
  ZSPoint->moveTo(Z_pos);
  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}
  deBuff();
  
  MSPoint->moveTo(M_ser);
  while(abs(MSPoint->distanceToGo())>0){stepperM.run();}
  deBuff();

}

//Unscrewing movement coordinated
long Unscrew() {

  long startup = ceil(0.1*stp1tour);
  long C_end = 0;
  RPC1.println("Start unscrewing of the cap");
  delay(1000);
  stepper_screwspd();
  RPC1.println("Preload the spring");
  ZSPoint->move(Z_prec);   //Z spring pre load
  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}

  delay(1000);
  RPC1.println("Start the rotation");
  deBuff();
  CSPoint->move(-ceil(0.75*stp1tour));     //Start rotation without Z movment
  //CSPoint->move(-2*stp1tour);     //Chuck rotation without Z movment
  while(abs(CSPoint->distanceToGo())<startup){stepperC.run();}  //After "Startup", Z follow the movment
  ZSPoint->moveTo(Zzero_offset);//Z is at 464000 must go to 200000
  delay(1000);
  RPC1.println("COntinue the rotation while going up");
  while(abs(CSPoint->distanceToGo())>0){runCZ;}
  C_end = ZSPoint->currentPosition();     //when chuck stop, we note Z position
  stepper_std();
  while(abs(ZSPoint->distanceToGo())>90000){stepperZ.run();}
  XSPoint->moveTo(Xzero_offset);
  while(abs(ZSPoint->distanceToGo())>0){runXZ;}
  while(abs(XSPoint->distanceToGo())>0){stepperX.run();}
  deBuff();

  RPC1.println("Removed the cap from the bottle");
  return C_end;
}
//Screwing
void reScrew(long C_start) {  //Takes as input the point where C stopped rotating 

 
  ZSPoint->moveTo(C_start);
  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}
  deBuff();
  stepper_screwspd();
  CSPoint->move(2*stp1tour);
  ZSPoint->moveTo(Z_pos+Z_prec);
  while(abs(CSPoint->distanceToGo())>0){runCZ;}
  ZSPoint->move(0);
  deBuff();
  stepper_std();
}

//Bringing back the Flask
void Bringback() {

  XSPoint->moveTo(Xzero_offset);
  while(abs(XSPoint->distanceToGo())>0){stepperX.run();}
  deBuff();
}


void untigh_up() {

  MSPoint->moveTo(Mzero_offset);
  while(abs(MSPoint->distanceToGo()>0)){stepperM.run();}
  deBuff();
  ZSPoint->moveTo(Zzero_offset);
  while(abs(ZSPoint->distanceToGo())>80000){stepperZ.run();}
  XSPoint->moveTo(Xzero_offset);
  while(abs(ZSPoint->distanceToGo())>0){runXZ;}
  deBuff();
  while(abs(XSPoint->distanceToGo())>0){stepperX.run();}
  deBuff();

}

long mmtostep(long dist, uint gear, float transmission) {
  return floor((dist / transmission) * motor_step_rot * gear);
}

bool isMVT() {
  return (ZSPoint->isRunning() || MSPoint->isRunning() || XSPoint->isRunning() || CSPoint->isRunning());
}

//Run all motors to their absolute targets
void runXZMC_toTargets(long target_x, long target_z,long target_m, long target_c) {
  
  long pos_x = XSPoint->currentPosition();
  long pos_z = ZSPoint->currentPosition();
  long pos_m = MSPoint->currentPosition();
  long pos_c = CSPoint->currentPosition();
  if(target_x == 0)target_x = pos_x;
  if(target_z == 0)target_z = pos_z;
  if(target_m == 0)target_m = pos_m;
  if(target_c == 0)target_c = pos_c;
  
  long dir_xm = (target_x-pos_x)*(target_m-pos_m);
  long dir_zc = (target_z-pos_z)*(target_c-pos_c);

  if(dir_xm>=0 && dir_zc>=0) {
    //X & M running in same dir
    //Can operate at the same time
    while(MSPoint->isRunning()||XSPoint->isRunning()||CSPoint->isRunning()||ZSPoint->isRunning()){
      runALL;
    }
  }
  else if(dir_xm>=0 && dir_zc <0){//Run XMZ & C
    while(ZSPoint->isRunning()){ //Run all but C until Z is done
      runXMZ;
    }
    while(MSPoint->isRunning()||CSPoint->isRunning()||ZSPoint->isRunning()){
      runALL;
    }
    
  }
  else if(dir_xm<0 && dir_zc >=0){//Run XCZ & M
    while(XSPoint->isRunning()){ //Run all but M until X is done
      runXCZ;
    }
    while(MSPoint->isRunning()||CSPoint->isRunning()||ZSPoint->isRunning()){
      runALL;
    }
  }
  else if(dir_xm<0 && dir_zc<0){ //Run XZ & MC
    while(XSPoint->isRunning()||ZSPoint->isRunning()){
      runXZ;
    }
    while(MSPoint->isRunning()||CSPoint->isRunning()){
      runMC;
    }
  }
}
