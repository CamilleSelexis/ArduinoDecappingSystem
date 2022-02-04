
//Now the command list must be defined
const commandList_t commands[] = {
  {"Set value",     setIntHandler,   "Set the value for the next command"},
  {"Move X",        MoveX,           "Move X by the previously set value in steps"}, 
  {"Move Z",        MoveZ,           "Move Z by the previously set value in steps"}, 
  {"Move C",        MoveC,           "Move C by the previously set value in steps"}, 
  {"Move M",        MoveM,           "Move M by the previously set value in steps"},
  {"Set speed X",   setSpeedX,       "Set Max Speed for X"},
  {"Set speed Z",   setSpeedZ,       "Set Max Speed for Z"},
  {"Set speed M",   setSpeedM,       "Set Max Speed for M"},
  {"Set speed C",   setSpeedC,       "Set Max Speed for C"},
  {"Stop",          stop,            "Disable all motors"                         },
  {"Enable",        go,              "Enable all motors"                          },
  {"Relay on",      relayOn,         "Turn the relay on"                          },
  {"Relay Off",     relayOff,        "Turn the relay off"                          },
  {"Get speed",     getSpeed,        "Get the Max speed sets on the different motors" },
};

//Initialisation function
void initialiseCommander(){
  comder.begin(&Serial, commands, sizeof(commands)); //start Commander on Serial
}

bool setIntHandler(Commander &Cmdr){
  if(Cmdr.getInt(commandVal)){
    Cmdr.print("Current value is set to ");
    Cmdr.println(commandVal);
  }
  //Cmdr.printDiagnostics();
  return 0;
}

bool MoveX(Commander &Cmdr){
  Cmdr.print("X moving by: ");
  Cmdr.println(commandVal);
  Xpointer->move(commandVal);
  return 0;
  }

bool MoveZ(Commander &Cmdr){
  Cmdr.print("Z moving by: ");
  Cmdr.println(commandVal);
  Zpointer->move(commandVal);
  return 0;
}

bool MoveC(Commander &Cmdr){
  Cmdr.print("C moving by: ");
  Cmdr.println(commandVal);
  Cpointer->move(commandVal);
  return 0;
}

bool MoveM(Commander &Cmdr){
  Cmdr.print("M moving by: ");
  Cmdr.println(commandVal);
  Mpointer->move(commandVal);
  return 0;
}

bool setSpeedX(Commander &Cmdr){
  Cmdr.print("Speed of X axis set at: ");
  Cmdr.println(commandVal);
  Xpointer->setMaxSpeed(commandVal);
  return 0;
}

bool setSpeedZ(Commander &Cmdr){
  Cmdr.print("Speed of Z axis set at: ");
  Cmdr.println(commandVal);
  Zpointer->setMaxSpeed(commandVal);
  return 0;
}

bool setSpeedM(Commander &Cmdr){
  Cmdr.print("Speed of M axis set at: ");
  Cmdr.println(commandVal);
  Mpointer->setMaxSpeed(commandVal);
  return 0;
}

bool setSpeedC(Commander &Cmdr){
  Cmdr.print("Speed of C axis set at: ");
  Cmdr.println(commandVal);
  Cpointer->setMaxSpeed(commandVal);
  return 0;
}

bool go(Commander &Cmdr){
  motorON;
  Cmdr.println("Motors enabled");
  return 0;
}

bool stop(Commander &Cmdr){
  motorOFF;
  Cmdr.println("Motors disabled");
  return 0;
}


bool relayOn(Commander &Cmdr) {
  digitalWrite(D7,HIGH);
  return 0;
}

bool relayOff(Commander &Cmdr) {
  digitalWrite(D7,LOW);
  return 0;
}
bool getSpeed(Commander &Cmdr) {
  float x_speed = Xpointer->maxSpeed();
  float z_speed = Zpointer->maxSpeed();
  float c_speed = Cpointer->maxSpeed();
  float m_speed = Mpointer ->maxSpeed();
  Cmdr.println("X speed = " + String(x_speed) + " /Z speed = " + String(z_speed) + " /C speed = " + String(c_speed) + " /M speed = " + String(m_speed));
}
