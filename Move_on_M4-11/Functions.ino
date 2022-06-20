//----------Movements------------//

//Ref all home Initialisation
void Ref()  {
  RPC.println("In the REF function");
  disableInt();
  //noInterrupts(); //Disable the interrupts during init as we want to touch the limits
  stopALL;
  stepper_init(); //Init steppers with init speeds
  RPC.println("Starting Ref");
  if(!digitalRead(pin_BZ)){ //Get away from Z baumer
    RPC.println("On the Z baumer, going away");
    ZSPoint->move(5000*micro_ratio);
    while(ZSPoint->isRunning()){stepperZ.run();}
  }
  MSPoint->move(2500*micro_ratio);        //Let go of a potential lid
  CSPoint->move(-2500*micro_ratio);
  while(MSPoint->isRunning()||CSPoint->isRunning()){runMC;}
  
  //----------------------------Set Z 0 Position-----------------------
  //refZ();
//  //----------------------------Set the origin for the claws
  //refM();
  refMZ();
  //-----------------------------Set the origin for the rotation
  enableInt();
  //interrupts(); //re-enable interrupts to do analog read
  refC();
  while(ZSPoint->isRunning()||MSPoint->isRunning()){
    runALL;
  }
  stopALL;
  stepper_std();

  MSPoint->moveTo(Mzero_offset);
  ZSPoint->moveTo(Zzero_offset);
  while(ZSPoint->isRunning()||MSPoint->isRunning()){
    runALL;
  }
  RPC.println("Init done");
}

void refZ() {
  ZSPoint->move(-400000*micro_ratio);    //Go up and search limit
  RPC.println("Going Up to find my limits");
  while(digitalRead(pin_BZ)){stepperZ.run();}//interrupt stop
  
  ZSPoint->move(3000*micro_ratio);        // leave the switch
  while(ZSPoint->isRunning()){stepperZ.run();}
  
  slow_initZ(); //slow the stepperZ
  ZSPoint->move(-5000*micro_ratio);       //Go back a second time
  while(digitalRead(pin_BZ)){stepperZ.run();}
  
  ZSPoint->setCurrentPosition(0);
  ZSPoint->move(1500*micro_ratio);    //Get away from the switch
  while(ZSPoint->isRunning()){stepperZ.run();}
  RPC.println("Ref Z is done");
}
void refM() {
  MSPoint->move(-1000000*micro_ratio);  //Go to first contact closure
  while(!digitalRead(pin_contact)){stepperM.run();}
  
  MSPoint->move(4000*micro_ratio);
  while(MSPoint->isRunning()){stepperM.run();}
  
  slow_initM();
  MSPoint->move(-5000*micro_ratio);    //Go back a second time
  while(!digitalRead(pin_contact)){stepperM.run();}  //Interupt contain the stp_Z move to 2000
  MSPoint->setCurrentPosition(0);
  MSPoint->move(1500*micro_ratio); //Go to an absolute position
  while(MSPoint->isRunning()){stepperM.run();}
  RPC.println("Ref M is done");
}

void refMZ() {
  RPC.println("Starting parrallel init for Z and M");
  
  ZSPoint->move(-1000000*micro_ratio);    //Go up and search limit
  MSPoint->move(-1000000*micro_ratio);  //Go to first contact closure
  while(ZSPoint->isRunning()||MSPoint->isRunning()){
    if(!digitalRead(pin_BZ)){ZSPoint->move(3000*micro_ratio);} //Change dir of Z
    if(digitalRead(pin_contact)){MSPoint->move(3000*micro_ratio);} // Change dir of M
    runMZ;
  }
  slow_initZ();
  slow_initM();
  ZSPoint->move(-5000*micro_ratio);       //Go back a second time
  MSPoint->move(-5000*micro_ratio);    //Go back a second time
  while(ZSPoint->isRunning()||MSPoint->isRunning()){
    if(!digitalRead(pin_BZ)){
      ZSPoint->setCurrentPosition(0);
//      ZSPoint->moveTo(2500*micro_ratio);
    }
    if(digitalRead(pin_contact)){
      MSPoint->setCurrentPosition(0);
//      MSPoint->moveTo(2500*micro_ratio);
    }
    runMZ;
  }
  ZSPoint->moveTo(2500*micro_ratio);
  MSPoint->moveTo(2500*micro_ratio);
  while(ZSPoint->isRunning()||MSPoint->isRunning()){
    runMZ;
  }
  RPC.println("Parrallel init done for Z and M");
}

void refC() {
  slow_initC();
  bool barrier= false;
  bool done_C = false;
  if(analogRead(pin_photo)>950){
    barrier = false;
    RPC.println("Not on the barrier, going CCW");
    CSPoint->move(-25000*micro_ratio);
  }
  else{
    barrier = true;
    RPC.println("In front of the barrier, going CW");
    CSPoint->move(50000*micro_ratio);
  }
  if(barrier){
    while(!done_C){
      CSPoint->run();
      if(analogRead(A0)>950){
        done_C = true;
      }
    }
  }
  else{ //Turn counter clockwise
    while(!done_C){
      //RPC.println("Going CCW");
      CSPoint->run();
      if(analogRead(A0)< 300){
        done_C = true; 
      }
    }
  }
  CSPoint->stop();
  CSPoint->setCurrentPosition(0);
  RPC.println("Rotation axe aligned");
}
void ToStandby(){
  RPC.println("Going back to standby position");
  ZSPoint->moveTo(Zzero_offset);
  MSPoint->moveTo(Mzero_offset);
  CSPoint->moveTo(Czero_offset);
  
  runZMC_toTargets(Zzero_offset,Mzero_offset,Czero_offset);
}

void Decap(){
  Get_flask();
  Align();
  Getdown();
  *PZ_screw = Unscrew();
}
//Get the bottle an bring it back under the arm
void Get_flask(){
  
  
  ZSPoint->moveTo(Z_inter);
  runZMC_toTargets(Z_inter,0,0);
  RPC.println("Decap in position");
}

//Tightening the claws according to the info from video
//does not tighten at all lol
//Align the claws with the cap
void Align() {
  //
  RPC.println("Align the claws with the cap");
  RPC.print("C pos received ");RPC.println(*PC_pos);
  CSPoint->moveTo(*PC_pos);
  while(CSPoint->isRunning()){stepperC.run();}
}

void Getdown(){ //Get down on the Cap and once arrived, tighten the claws
  
  
  RPC.println("Going down, and then tightening the claws");
  ZSPoint->moveTo(Z_pos);
  while(ZSPoint->isRunning()){stepperZ.run();}
  
  
  MSPoint->moveTo(M_ser);
  while(MSPoint->isRunning()){stepperM.run();}
  

}

//Unscrewing movement coordinated
long Unscrew() {

  long startup = ceil(0.1*stp1tour);
  long Z_end = 0; //End position of Z
  RPC.println("Start unscrewing of the cap");
  stepper_screwspd();
  RPC.println("Preload the spring");
  ZSPoint->move(Z_prec);   //Z spring pre load
  while(ZSPoint->isRunning()){stepperZ.run();}
  RPC.println("Start the rotation");
  
  CSPoint->move(-ceil(0.8*stp1tour));     //Start rotation without Z movment
  while(abs(CSPoint->distanceToGo())<startup){stepperC.run();}  //After "Startup", Z follow the movment
  ZSPoint->moveTo(Zzero_offset);//Z is at 464000 must go to 200000
  RPC.println("Continue the rotation while going up");
  while(CSPoint->isRunning()){runCZ;}
  Z_end = ZSPoint->currentPosition();     //when chuck stop, we note Z position
  stepper_std();
  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}
  while(ZSPoint->isRunning()){stepperZ.run();}
  

  RPC.println("Removed the cap from the bottle");
  return Z_end;
}
void Recap() {
  Get_flask();
  reScrew(Z_screw);
  ToStandby();
}
//Screwing
void reScrew(long Z_screw) {  //Takes as input the point where C stopped rotating 

  RPC.println("Go back to screw position");
  ZSPoint->moveTo(Z_screw);
  while(ZSPoint->isRunning()){stepperZ.run();}
  
  stepper_screwspd();
  RPC.println("Starting the screwing");
  CSPoint->move(0.8*stp1tour);
  ZSPoint->moveTo(Z_pos+Z_prec);
  while(CSPoint->isRunning()){runCZ;}
  ZSPoint->move(0);
  
  stepper_std();
  RPC.println("Cap screwed back, opening claws and then going away");
  //Untighten the claws
  MSPoint->moveTo(Mzero_offset);
  while(MSPoint->isRunning()){stepperM.run();}
  
  //Go up a bit
  ZSPoint->moveTo(Zzero_offset);
  while(abs(ZSPoint->distanceToGo())>80000*micro_ratio){stepperZ.run();}
}

//Unengage on the X axis
void Bringback() {
  
}


void untigh_up() {

  MSPoint->moveTo(Mzero_offset);
  while(abs(MSPoint->distanceToGo()>0)){stepperM.run();}
  ZSPoint->moveTo(Zzero_offset);
  while(abs(ZSPoint->distanceToGo())>0){stepperZ.run();}

}

//long mmtostep(long dist, uint gear, float transmission) {
//  return floor((dist / transmission) * motor_step_rot * gear);
//}

bool isMVT() {
  return (ZSPoint->isRunning()||MSPoint->isRunning()||CSPoint->isRunning());
}

//Run all motors to their absolute targets
void runZMC_toTargets(long target_z,long target_m, long target_c) {
  
  long pos_z = ZSPoint->currentPosition();
  long pos_m = MSPoint->currentPosition();
  long pos_c = CSPoint->currentPosition();
  if(target_z == 0)target_z = pos_z;
  if(target_m == 0)target_m = pos_m;
  if(target_c == 0)target_c = pos_c;
  
  long dir_m = (target_m-pos_m);
  long dir_zc = (target_z-pos_z)*(target_c-pos_c);

  if(dir_m>=0 && dir_zc>=0) {
    //X & M running in same dir
    //Can operate at the same time
    while(MSPoint->isRunning()||CSPoint->isRunning()||ZSPoint->isRunning()){
      runALL;
    }
  }
  else if(dir_m>=0 && dir_zc <0){//Run XMZ & C
    while(ZSPoint->isRunning()){ //Run all but C until Z is done
      runMZ;
    }
    while(MSPoint->isRunning()||CSPoint->isRunning()||ZSPoint->isRunning()){
      runALL;
    }
    
  }
  else if(dir_m<0 && dir_zc >=0){//Run XCZ & M
    while(MSPoint->isRunning()||CSPoint->isRunning()||ZSPoint->isRunning()){
      runALL;
    }
  }
  else if(dir_m<0 && dir_zc<0){ //Run XZ & MC
    while(ZSPoint->isRunning()){
      stepperZ.run();
    }
    while(MSPoint->isRunning()||CSPoint->isRunning()){
      runMC;
    }
  }
}

void MoveZ(int32_t target){
  RPC.println("Will move Z by : " + String(target));
  ZSPoint->move(target*micro_ratio);
  while(ZSPoint->isRunning()){stepperZ.run();}
}
void MoveM(int32_t target){
  RPC.println("Will move M by : " + String(target));
  MSPoint->move(target*micro_ratio);
  while(MSPoint->isRunning()){stepperM.run();}
}
void MoveC(int32_t target){
  RPC.println("Will move C by : " + String(target));
  CSPoint->move(target*micro_ratio);
  while(CSPoint->isRunning()){stepperC.run();}
}
