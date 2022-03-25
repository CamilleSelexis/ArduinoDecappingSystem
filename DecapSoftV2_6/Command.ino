//All the commands called by ethernet calls. 
//They are a list of RPC commands, Wait(), communication and minors hardware control (crydom, leds)

void refAllHome(){ //Initialisation routine
  Serial.println("The machine will now ref all its axis...");
  mvt_in;        //Mvt led on
  motorON;      //Motor enable
  *Pworking = RPC.call("Ref").as<bool>();
  Wait();
  motorOFF;     //motor disable
  mvt_out;       //Led mvt off
  Serial.println("RefAllHome complete.");
  *PisInit = true;
}

void Decap(){
  if(!isInit){Serial.println("The machine is not intialized");}else{
  if(C_start){
    Serial.println("The machine cannot decap with a prior decaping");
  }else{
    Serial.println("The machine will now start the decaping routine. Keep clear");
    mvt_in;
    motorON;
    digitalWrite(pin_crydom,HIGH);
    *Pworking = RPC.call("Get_flask").as<bool>();
    Wait();
    motorOFF;
    long C_pos = finalPos();

    Serial.print("Camera capture returned the value : ");
    Serial.println(C_pos);
    motorON;
    *Pworking = RPC.call("Align",C_pos).as<bool>();
    Wait();
    *Pworking = RPC.call("GetDown").as<bool>();
    Wait();
    *Pworking = RPC.call("Unscrew").as<bool>();
    Wait();
    *CstartPoint = true;
    //*Pworking = RPC.call("Bringback").as<bool>();
    //Wait();
    motorOFF;
    mvt_out;
    digitalWrite(pin_crydom,LOW);
    Serial.println("Decaping finished");
    digitalWrite(LEDG,LOFF);
    digitalWrite(LEDB,LON); //LEDB signals that a cap is being held

  }
}}

          
void Recap(){
  if(!C_start){
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
    *CstartPoint = false;
    *Pworking = RPC.call("untigh_up").as<bool>();
    Wait();
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recaping finished");
    *Pworking = RPC.call("RefM").as<bool>();
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
    *CstartPoint = true;
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
    *CstartPoint = false;
    *Pworking = RPC.call("untigh_up").as<bool>();
    Wait();
    digitalWrite(pin_crydom,LOW);
    Serial.println("Recaping finished");
    *Pworking = RPC.call("RefM").as<bool>();
    Wait();
    motorOFF;
    mvt_out;
  }

void Wait() {
  delay(500);
  RPC.flush();
  while(!(RPC.parseInt(SKIP_NONE)==1)){
    delay(100);
    String buffer = "";
    while (RPC.available()) {
      buffer += (char)RPC.read(); // Fill the buffer with characters
    }
  
    if (buffer.length() > 0) {
      Serial.print(buffer);
    }
  } 
  *Pworking = RPC.call("Backto1").as<bool>();
  delay(100);
  }
  
