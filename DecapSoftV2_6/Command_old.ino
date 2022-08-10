/*//All the commands called by ethernet calls. 
//They are a list of RPC commands, Wait(), communication and minors hardware control (crydom, leds)

void refAllHome(){ //Initialisation routine
  long init_start_time = millis();
  Serial.println("The machine will now ref all its axis...");
  mvt_in;        //Mvt led on
  motorON;      //Motor enable
  *Pworking = RPC.call("Ref").as<bool>();
  Wait();
  motorOFF;     //motor disable
  *capHeld_pntr = false;
  mvt_out;       //Led mvt off
  Serial.println("Init routine took" + String(millis()-init_start_time)+ " ms to complete");
  Serial.println("RefAllHome complete.");
  
  *isInit_pntr = true;
}
void GoToStandby(){
  Serial.println("Going back to standby");
  mvt_in;        //Mvt led on
  motorON;      //Motor enable
  *Pworking = RPC.call("GoToStandby").as<bool>();
  Wait();
  motorOFF;     //motor disable
  mvt_out;       //Led mvt off
  *capHeld_pntr = false;
  digitalWrite(LEDB,LOFF);
  Serial.println("Standby reached");
}
void Decap(){
  long decap_start_time = millis();
  int state = getStatus();
  if(state != 1){
    Serial.print("Cannot start decap, status = ");Serial.println(state);
    return;
  }
  else{
  
    Serial.println("The machine will now start the decaping routine. Keep clear");
    long C_pos = finalPos();

    Serial.print("Camera capture returned the value : ");
    Serial.println(C_pos);
    mvt_in;
    motorON;
    digitalWrite(pin_crydom,HIGH);
    *Pworking = true;
    if(!RPC.call("Decap",(int)C_pos).as<int>())
      {Serial.println("Error calling Decap");}     
    Wait();
    motorOFF;
    mvt_out;
    *capHeld_pntr = true;
    *capHeld_pntr = true;
    digitalWrite(pin_crydom,LOW);
    Serial.println("Decaping finished");
    Serial.println("Decap routine took" + String(millis()-decap_start_time) + " ms to complete");
    digitalWrite(LEDG,LON);
    digitalWrite(LEDB,LON); //LEDB signals that a cap is being held

  }
}


void Recap(){
  long recap_start_time = millis();
  int state = getStatus();
  if(state != 2){
    Serial.print("The machine cannot recap, status should be 2, current status = ")Serial.println(state);
  }else{
    Serial.println("The machine will now start the recaping routine. Keep clear");
    digitalWrite(pin_crydom,HIGH);
    mvt_in;
    digitalWrite(LEDB,LOFF);
    motorON;
    *Pworking = RPC.call("Recap").as<bool>();
    Wait();
    *capHeld_pntr = false;
    motorOFF;
    mvt_out;
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recap routine took" + String(millis()-recap_start_time) + " ms to complete");
    Serial.println("Recaping finished");
  }
}

void Align(){
  long decap_start_time = millis();
      Serial.println("The machine will now start the alignment routine. Keep clear");
      long C_pos = finalPos();
      int temp = 0;
      Serial.print("Camera capture returned the value : ");
      Serial.println(C_pos);
      mvt_in;
      motorON;
      digitalWrite(pin_crydom,HIGH);
      *Pworking = true;
      temp = RPC.call("Align",(int)C_pos).as<int>();
      Wait();
      motorOFF;
      mvt_out;
      digitalWrite(pin_crydom,LOW);
      Serial.println("Alignement done");
      Serial.println("Align routine took" + String(millis()-decap_start_time) + " ms to complete");
      digitalWrite(LEDG,LON);
      digitalWrite(LEDB,LON); //LEDB signals that a cap is being held
} 

          
void Recap_old(){
  long recap_start_time = millis();
  if(!capHeld){
    Serial.println("The machine cannot recap without prior decaping");
  }else{
    Serial.println("The machine will now start the recaping routine. Keep clear");
    digitalWrite(pin_crydom,HIGH);
    mvt_in;
    digitalWrite(LEDB,LOFF);
    motorON;
    *Pworking = RPC.call("Get_flask").as<bool>();
    Wait();
    *Pworking = RPC.call("reScrew").as<bool>();
    Wait();
    *capHeld_pntr = false;
    *Pworking = RPC.call("untigh_up").as<bool>();
    Wait();
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recap routine took" + String(millis()-recap_start_time) + " ms to complete");
    Serial.println("Recaping finished");
    *Pworking = RPC.call("RefM").as<bool>();
    Wait();
    motorOFF;
    mvt_out;
  }
}
void SudoDecap(){
    long C_pos = 0;
Serial.println("The machine will now start the decaping routine. Keep clear");
    mvt_in;
    motorON;
    digitalWrite(pin_crydom,HIGH);
    *Pworking = RPC.call("Get_flask").as<bool>();
    Wait();
    *Pworking = RPC.call("GetDown").as<bool>();
    //long C_pos = GetPos();
    //Serial.print("Camera capture returned the value :");
    //Serial.println(C_pos);

    //Wait();
    //*Pworking = RPC.call("Align",C_pos).as<bool>();
    Wait();
    *Pworking = RPC.call("Unscrew").as<bool>();
    Wait();
    *capHeld_pntr = true;
    *Pworking = RPC.call("Bringback").as<bool>();
    Wait();
    motorOFF;
    mvt_out;
    digitalWrite(pin_crydom,LOW);
    Serial.println("Decaping finished");
    digitalWrite(LEDG,LOFF);
    digitalWrite(LEDB,LON);

}

void SudoRecap(){

    Serial.println("The machine will now start the recaping routine. Keep clear");
    digitalWrite(pin_crydom,HIGH);
    mvt_in;
    digitalWrite(LEDB,LOFF);
    motorON;
    *Pworking = RPC.call("Get_flask").as<bool>();
    Wait();
    *Pworking = RPC.call("reScrew").as<bool>();
    Wait();
    *capHeld_pntr = false;
    *Pworking = RPC.call("untigh_up").as<bool>();
    Wait();
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recaping finished");
    *Pworking = RPC.call("RefM").as<bool>();
    Wait();
    motorOFF;
    mvt_out;
  }
  
void Move(char target, byte val0,byte val1, byte val2, byte val3){
  if(target == 'Z'){
    byte value = RPC.call("MoveZ",val0,val1,val2,val3).as<byte>();
  }
  if(target == 'M'){
    byte value = RPC.call("MoveM",val0,val1,val2,val3).as<byte>();
  }
    if(target == 'C'){
    byte value = RPC.call("MoveC",val0,val1,val2,val3).as<byte>();
  }
}
void Speed(char target, byte val0,byte val1, byte val2, byte val3){
  
}

void Wait() {
  delay(500);
  RPC.flush();
  while(*M4work_pntr){
    delay(100);
    String buffer = "";
    while (RPC.available()) {
      buffer += (char)RPC.read(); // Fill the buffer with characters
    }
  
    if (buffer.length() > 0) {
      Serial.print(buffer);
    }

  }
  delay(100);
}
  
void Status(){
  if(capHeld){
    Serial.println("I have a cap in my claws");
  } else if(isInit) {
    Serial.println("Machine available and initialised");
    } else {Serial.println("Machine available but not initialised");}
}
//This function returns the status of the decapper
//The status depends on 4 booleans, HS, capHeld, isInit and M4work
//0 : decapper out of order
//1 : decapper available
//2 : decapper used
//3 : decapper not init
//4 : decapper working
int getStatus(){
  int statusDecap = 0;
  if(HS)statusDecap = 0; //Decapper HS
  else if(M4work) statusDecap = 4; //Decapper working
  else if(!isInit) statusDecap = 3; //Decapper not init
  else if(capHeld) statusDecap = 2; //Decapper has a cap held, ready for recap
  else statusDecap = 1; //Decapper is ready
  
  return statusDecap; 
}

void resetFunc(void) {
  unsigned long *registerAddr;
  registerAddr = (unsigned long *)0xE000ED0C; //Writes to the AIRCR register of the stm32h747 to software restet the arduino
  //It is a 32 bit register set bit 2 to request a reset and write 0x05FA to enable the write
  //See Arm® v7-M Architecture Reference Manual for more information
  *registerAddr = (unsigned long) 0x05FA0304;
}*/
