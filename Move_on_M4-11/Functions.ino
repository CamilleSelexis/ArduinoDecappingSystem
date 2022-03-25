//----------Movements------------//

//Ref all home Initialisation
void Ref()  {
  noInterrupts(); //Disable the interrupts during init as we want to touch the limits
  stopALL;
  deBuff();
  stepper_init(); //Init steppers with init speeds
  RPC1.println("Starting Ref");
  if(!digitalRead(pin_BZ)){ //Get away from Z baumer
    RPC1.println("On the Z baumer, going away");
    ZSPoint->move(5000*micro_ratio);
    while(ZSPoint->isRunning()){stepperZ.run();}
  }
  if(!digitalRead(pin_BX)){ //Get away from X baumer
    RPC1.println("On the X baumer, going away");
    XSPoint->move(2500*micro_ratio);
    while(XSPoint->isRunning()){stepperX.run();}
  }
  MSPoint->move(4000*micro_ratio);        //Let go of a potential lid
  CSPoint->move(-2500*micro_ratio);
  while(MSPoint->isRunning()||CSPoint->isRunning()){runMC;}
  deBuff();
  //----------------------------Set Z 0 Position-----------------------
  //refZ();
//  //----------------------------------Set X 0 Position -----------------
  refX();
//  //----------------------------Set the origin for the claws
  //refM();
  refMZ();
  //-----------------------------Set the origin for the rotation
  interrupts(); //re-enable interrupts to do analog read
  refC();
  while(XSPoint->isRunning()||ZSPoint->isRunning()||MSPoint->isRunning()){
    runALL;
  }
  stopALL;
  stepper_std();

  MSPoint->moveTo(Mzero_offset);
  ZSPoint->moveTo(Zzero_offset);
  XSPoint->moveTo(Xzero_offset); 
  runXZMC_toTargets(Xzero_offset,Zzero_offset,Mzero_offset,0);
  RPC1.println("Init done");
}

void refZ() {
  ZSPoint->move(-400000*micro_ratio);    //Go up and search limit
  RPC1.println("Going Up to find my limits");
  while(digitalRead(pin_BZ)){stepperZ.run();}//interrupt stop
  deBuff();
  ZSPoint->move(3000*micro_ratio);        // leave the switch
  while(ZSPoint->isRunning()){stepperZ.run();}
  deBuff();
  slow_initZ(); //slow the stepperZ
  ZSPoint->move(-5000*micro_ratio);       //Go back a second time
  while(digitalRead(pin_BZ)){stepperZ.run();}
  deBuff();
  ZSPoint->setCurrentPosition(0);
  ZSPoint->move(1500*micro_ratio);    //Get away from the switch
  while(ZSPoint->isRunning()){stepperZ.run();}
  RPC1.println("Ref Z is done");
}
void refX() {
  deBuff();
  XSPoint->move(-1000000*micro_ratio);    //Go far from the platform, search limit
  RPC1.println("Going away to find my limits");
  while(digitalRead(pin_BX)){stepperX.run();}
  deBuff();
  XSPoint->move(1500*micro_ratio); //Get away from the Baumer X
  while(XSPoint->isRunning()){stepperX.run();}
  deBuff();
  slow_initX();
  XSPoint->move(-5000*micro_ratio);    //Go back a second time
  while(digitalRead(pin_BX)){stepperX.run();}
  deBuff();
  XSPoint->setCurrentPosition(0);
  XSPoint->move(3000*micro_ratio);
  while(XSPoint->isRunning()){stepperX.run();}
  deBuff();
  RPC1.println("Ref X is done");
}
void refM() {
  MSPoint->move(-1000000*micro_ratio);  //Go to first contact closure
  while(!digitalRead(pin_contact)){stepperM.run();}
  deBuff();
  MSPoint->move(4000*micro_ratio);
  while(MSPoint->isRunning()){stepperM.run();}
  deBuff();
  slow_initM();
  MSPoint->move(-5000*micro_ratio);    //Go back a second time
  while(!digitalRead(pin_contact)){stepperM.run();}  //Interupt contain the stp_Z move to 2000
  MSPoint->setCurrentPosition(0);
  MSPoint->move(1500*micro_ratio); //Go to an absolute position
  while(MSPoint->isRunning()){stepperM.run();}
  RPC1.println("Ref M is done");
}

void refMZ() {
  RPC1.println("Starting parrallel init for Z and M");
  deBuff();
  ZSPoint->move(-1000000*micro_ratio);    //Go up and search limit
  MSPoint->move(-1000000*micro_ratio);  //Go to first contact closure
  bool phase_1 = true;
  while(phase_1){
    if(!digitalRead(pin_BZ)){ZSPoint->move(3000*micro_ratio);} //Change dir of Z
    if(digitalRead(pin_contact)){MSPoint->move(4000*micro_ratio);} // Change dir of M
    runMZ;
    if(!ZSPoint->isRunning()&&!MSPoint->isRunning()){
      phase_1 = false;
    }
  }
  deBuff();
  slow_initZ();
  slow_initM();
  ZSPoint->move(-5000*micro_ratio);       //Go back a second time
  MSPoint->move(-5000*micro_ratio);    //Go back a second time
  bool phase_2 = true;
  while(phase_2){
    if(!digitalRead(pin_BZ)){ZSPoint->setCurrentPosition(0);ZSPoint->move(0);}
    if(digitalRead(pin_contact)){MSPoint->setCurrentPosition(0);MSPoint->move(0);}
    runMZ;
    if(!ZSPoint->isRunning()&&!MSPoint->isRunning()){
      phase_2 = false;
    }
  }
  deBuff();
  ZSPoint->move(1500*micro_ratio);       //Go away
  MSPoint->move(2500*micro_ratio);    //Go away
  while(ZSPoint->isRunning()&&MSPoint->isRunning()){
    runMZ;
  }
  RPC1.println("Parrallel init done for Z and M");
}

void refC() {
  slow_initC();
  bool barrier= false;
  bool done_C = false;
  if(analogRead(pin_photo)>950){
    barrier = false;
    RPC1.println("Not on the barrier, going CCW");
    CSPoint->move(-25000*micro_ratio);
  }
  else{
    barrier = true;
    RPC1.println("In front of the barrier, going CW");
    CSPoint->move(50000*micro_ratio);
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
void ToStandby(){
  RPC1.println("Going back to standby position");
  XSPoint->moveTo(Xzero_offset);
  ZSPoint->moveTo(Zzero_offset);
  MSPoint->moveTo(Mzero_offset);
  CSPoint->moveTo(Czero_offset);
  deBuff();
  runXZMC_toTargets(Xzero_offset,Zzero_offset,Mzero_offset,Czero_offset);
}

void Decap(long C_pos){
  Get_flask();
  Align(C_pos);
  Getdown();
  *PZ_screw = Unscrew();
}
//Get the bottle an bring it back under the arm
void Get_flask(){
  
  deBuff();
  XSPoint->moveTo(X_pos);
  ZSPoint->moveTo(Z_inter);
  runXZMC_toTargets(X_pos,Z_inter,0,0);
  RPC1.println("Decap in position");
}

//Tightening the claws according to the info from video
//does not tighten at all lol
//Align the claws with the cap
void Align(long C_pos) {
  
  deBuff();
  RPC1.println("Align the claws with the cap");
  RPC1.println("C pos received " + String(C_pos));
  CSPoint->moveTo(C_pos);
  while(CSPoint->isRunning()){stepperC.run();}
}

void Getdown(){ //Get down on the Cap and once arrived, tighten the claws
  
  deBuff();
  RPC1.println("Going down, and then tightening the claws");
  ZSPoint->moveTo(Z_pos);
  while(ZSPoint->isRunning()){stepperZ.run();}
  deBuff();
  
  MSPoint->moveTo(M_ser);
  while(MSPoint->isRunning()){stepperM.run();}
  deBuff();

}

//Unscrewing movement coordinated
long Unscrew() {

  long startup = ceil(0.1*stp1tour);
  long Z_end = 0; //End position of Z
  RPC1.println("Start unscrewing of the cap");
  stepper_screwspd();
  RPC1.println("Preload the spring");
  ZSPoint->move(Z_prec);   //Z spring pre load
  while(ZSPoint->isRunning()){stepperZ.run();}
  RPC1.println("Start the rotation");
  deBuff();
  CSPoint->move(-ceil(0.8*stp1tour));     //Start rotation without Z movment
  while(abs(CSPoint->distanceToGo())<startup){stepperC.run();}  //After "Startup", Z follow the movment
  ZSPoint->moveTo(Zzero_offset);//Z is at 464000 must go to 200000
  RPC1.println("Continue the rotation while going up");
  while(CSPoint->isRunning()){runCZ;}
  Z_end = ZSPoint->currentPosition();     //when chuck stop, we note Z position
  stepper_std();
  while(abs(ZSPoint->distanceToGo())>80000*micro_ratio){stepperZ.run();}
  XSPoint->moveTo(Xzero_offset);
  while(ZSPoint->isRunning()||XSPoint->isRunning()){runXZ;}
  deBuff();

  RPC1.println("Removed the cap from the bottle");
  return Z_end;
}
void Recap() {
  Get_flask();
  reScrew(Z_screw);
  ToStandby();
}
//Screwing
void reScrew(long Z_screw) {  //Takes as input the point where C stopped rotating 

  RPC1.println("Go back to screw position");
  ZSPoint->moveTo(Z_screw);
  while(ZSPoint->isRunning()){stepperZ.run();}
  deBuff();
  stepper_screwspd();
  RPC1.println("Starting the screwing");
  CSPoint->move(0.8*stp1tour);
  ZSPoint->moveTo(Z_pos+Z_prec);
  while(CSPoint->isRunning()){runCZ;}
  ZSPoint->move(0);
  deBuff();
  stepper_std();
  RPC1.println("Cap screwed back, opening claws and then going away");
  //Untighten the claws
  MSPoint->moveTo(Mzero_offset);
  while(MSPoint->isRunning()){stepperM.run();}
  deBuff();
  //Go up a bit
  ZSPoint->moveTo(Zzero_offset);
  while(abs(ZSPoint->distanceToGo())>80000*micro_ratio){stepperZ.run();}
}

//Unengage on the X axis
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
  while(abs(ZSPoint->distanceToGo())>40000*micro_ratio){stepperZ.run();}
  XSPoint->moveTo(Xzero_offset);
  while(abs(ZSPoint->distanceToGo())>0){runXZ;}
  deBuff();
  while(abs(XSPoint->distanceToGo())>0){stepperX.run();}
  deBuff();

}

//long mmtostep(long dist, uint gear, float transmission) {
//  return floor((dist / transmission) * motor_step_rot * gear);
//}

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
