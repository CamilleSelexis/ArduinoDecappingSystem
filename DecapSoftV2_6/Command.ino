//All the commands called by ethernet calls. 
//They are a list of RPC1 commands, Wait(), communication and minors hardware control (crydom, leds)

void refAllHome(){ //Initialisation routine
  long init_start_time = millis();
  Serial.println("The machine will now ref all its axis...");
  mvt_in;        //Mvt led on
  motorON;      //Motor enable
  *Pworking = RPC1.call("Ref").as<bool>();
  Wait();
  motorOFF;     //motor disable
  mvt_out;       //Led mvt off
  Serial.println("Init routine took" + String(millis()-init_start_time)+ " ms to complete");
  Serial.println("RefAllHome complete.");
  
  *PisInit = true;
}
void GoToStandby(){
  Serial.println("Going back to standby");
  mvt_in;        //Mvt led on
  motorON;      //Motor enable
  *Pworking = RPC1.call("GoToStandby").as<bool>();
  Wait();
  motorOFF;     //motor disable
  mvt_out;       //Led mvt off
  *CstartPoint = false;
  digitalWrite(LEDB,LOFF);
  Serial.println("Standby reached");
}
void Decap(){
  long decap_start_time = millis();
  if(!isInit){
    Serial.println("The machine is not intialized");
    return; //if not init then do nothing
  }
  else{
    if(C_start){
      Serial.println("The machine cannot decap with a prior decaping");
    }else{
      Serial.println("The machine will now start the decaping routine. Keep clear");
      long C_pos = finalPos();
  
      Serial.print("Camera capture returned the value : ");
      Serial.println(C_pos);
      mvt_in;
      motorON;
      digitalWrite(pin_crydom,HIGH);
      *Pworking = RPC1.call("Decap",C_pos).as<long>();
      Wait();
      motorOFF;
      mvt_out;
      *CstartPoint = true;
      digitalWrite(pin_crydom,LOW);
      Serial.println("Decaping finished");
      Serial.println("Decap routine took" + String(millis()-decap_start_time) + " ms to complete");
      digitalWrite(LEDG,LON);
      digitalWrite(LEDB,LON); //LEDB signals that a cap is being held
  
    }
}}
void Recap(){
  long recap_start_time = millis();
  if(!C_start){
    Serial.println("The machine cannot recap without prior decaping");
  }else{
    Serial.println("The machine will now start the recaping routine. Keep clear");
    digitalWrite(pin_crydom,HIGH);
    mvt_in;
    digitalWrite(LEDB,LOFF);
    motorON;
    *Pworking = RPC1.call("Recap").as<bool>();
    Wait();
    *CstartPoint = false;
    motorOFF;
    mvt_out;
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recap routine took" + String(millis()-recap_start_time) + " ms to complete");
    Serial.println("Recaping finished");
  }
}
void Align(){
  long decap_start_time = millis();
  if(!isInit){
    Serial.println("The machine is not intialized");
    return; //if not init then do nothing
  }
  else{
    if(C_start){
      Serial.println("The machine cannot decap with a prior decaping");
    }
    else{
      Serial.println("The machine will now start the alignment routine. Keep clear");
      long C_pos = finalPos();
  
      Serial.print("Camera capture returned the value : ");
      Serial.println(C_pos);
      mvt_in;
      motorON;
      digitalWrite(pin_crydom,HIGH);
      *Pworking = RPC1.call("Align",C_pos).as<long>();
      Wait();
      motorOFF;
      mvt_out;
      *CstartPoint = true;
      digitalWrite(pin_crydom,LOW);
      Serial.println("Alignement done");
      Serial.println("Align routine took" + String(millis()-decap_start_time) + " ms to complete");
      digitalWrite(LEDG,LON);
      digitalWrite(LEDB,LON); //LEDB signals that a cap is being held
  
    }
  }
} 

          
void Recap_old(){
  long recap_start_time = millis();
  if(!C_start){
    Serial.println("The machine cannot recap without prior decaping");
  }else{
    Serial.println("The machine will now start the recaping routine. Keep clear");
    digitalWrite(pin_crydom,HIGH);
    mvt_in;
    digitalWrite(LEDB,LOFF);
    motorON;
    *Pworking = RPC1.call("Get_flask").as<bool>();
    Wait();
    *Pworking = RPC1.call("reScrew").as<bool>();
    Wait();
    *CstartPoint = false;
    *Pworking = RPC1.call("untigh_up").as<bool>();
    Wait();
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recap routine took" + String(millis()-recap_start_time) + " ms to complete");
    Serial.println("Recaping finished");
    *Pworking = RPC1.call("RefM").as<bool>();
    Wait();
    motorOFF;
    mvt_out;
  }
}

void Status(){
  if(C_start){
    Serial.println("I have a cap in my claws");
  } else if(isInit) {
    Serial.println("Machine available and initialised");
    } else {Serial.println("Machine available but not initialised");}
  

  }


void SudoDecap(){
    long C_pos = 0;
Serial.println("The machine will now start the decaping routine. Keep clear");
    mvt_in;
    motorON;
    digitalWrite(pin_crydom,HIGH);
    *Pworking = RPC1.call("Get_flask").as<bool>();
    Wait();
    *Pworking = RPC1.call("GetDown").as<bool>();
    //long C_pos = GetPos();
    //Serial.print("Camera capture returned the value :");
    //Serial.println(C_pos);

    //Wait();
    //*Pworking = RPC1.call("Align",C_pos).as<bool>();
    Wait();
    *Pworking = RPC1.call("Unscrew").as<bool>();
    Wait();
    *CstartPoint = true;
    *Pworking = RPC1.call("Bringback").as<bool>();
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
    *Pworking = RPC1.call("Get_flask").as<bool>();
    Wait();
    *Pworking = RPC1.call("reScrew").as<bool>();
    Wait();
    *CstartPoint = false;
    *Pworking = RPC1.call("untigh_up").as<bool>();
    Wait();
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recaping finished");
    *Pworking = RPC1.call("RefM").as<bool>();
    Wait();
    motorOFF;
    mvt_out;
  }

void Wait() {
  delay(500);
  RPC1.flush();
  while(!(RPC1.parseInt(SKIP_NONE)==1)){
    delay(100);
    String buffer = "";
    while (RPC1.available()) {
      buffer += (char)RPC1.read(); // Fill the buffer with characters
    }
  
    if (buffer.length() > 0) {
      Serial.print(buffer);
    }
  } 
  *Pworking = RPC1.call("Backto1").as<bool>();
  delay(100);
  }
  
