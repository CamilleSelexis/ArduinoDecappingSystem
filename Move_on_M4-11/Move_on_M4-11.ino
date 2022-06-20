/*
Move_on_M4 v6 by Jérémie Pochon
Selexis Genève 20.08.2021
To run on an Arduino Portenta H7 with a vision shield
M4 core
In coordination with DecapSoft on M7 core
Removed X Axis
*/

#include <AccelStepper.h>
#include "Arduino.h"
#include "RPC.h"
using namespace rtos;

#define mvt_in  digitalWrite(LEDG,LOFF);digitalWrite(LEDR,LON)
#define mvt_out digitalWrite(LEDR,LOFF);digitalWrite(LEDG,LON)

#define runCZ    stepperC.run();stepperZ.run()
#define runMC    stepperM.run();stepperC.run()
#define runMZ    stepperM.run();stepperZ.run()
#define runALL   stepperC.run();stepperM.run();stepperZ.run()
#define stopALL  stepperC.stop();stepperM.stop();stepperZ.stop();
#define pin_contact A2 //Used on M
#define pin_photo   A0 //Photodetecteur C

#define pin_BZ      D0
#define DIR_MX      D13
#define DIR_CZ      D14

const int LON = LOW; // Voltage level is inverted for the LED
const int LOFF = HIGH;
const int motor_step_rot = 200;  //Number of steps in rot of the motors
const int micro_ratio = 4; //Microsteps ratio
const uint MPW = 20;        //Min pulse width in us

//uint32_t Zzero_offset = 140000*micro_ratio; //Standby position Z [steps]
const long Z_pos = 227500*micro_ratio;   //Position of the claws at the cap level
const long Z_inter = ceil(0.9*Z_pos);//Zzero_offset+90000*micro_ratio; //Intermediate position
const long Z_prec = 500*micro_ratio;   //Precontrainte on the spring
const long Zgear = 19;        //Z motor gearbox


//uint32_t Czero_offset = 1*micro_ratio;    //Standby position C [tours]
const long Cgear = 139;         //C motor gearbox
const float Ctrans = 1.25;       //C secondary gear ratio
long stp1tour;       //Number of step in one C rotation.

const uint pasLBW = 8;     //Pas de vis du Labware

//uint32_t Mzero_offset = 63000*micro_ratio ; //63000*micro_ratio; //Standby position M
const long M_open = 63000*micro_ratio; //Open position for the claws
const uint Mgear = 100;       //M motor gearbox
const long M_ser = 52000*micro_ratio;         //Active position M

volatile int8_t status = -1;
volatile int8_t* Pstatus = &status;

//Position obtained with the picture from the camera
int C_pos = 0;
int *PC_pos = &C_pos;

//Z position where the unscrewing finishes and the screwing must start
long Z_screw;
long* PZ_screw = &Z_screw;

//-----Stepper Definition------//

int32_t Zzero_offset = 140000*micro_ratio;
int32_t Mzero_offset = 63000*micro_ratio;
int32_t Czero_offset = 1*micro_ratio;
int32_t Zspeed = 1000*micro_ratio;
int32_t Mspeed = 1000*micro_ratio;
int32_t Cspeed = 1000*micro_ratio;
int32_t Zacc = 500*micro_ratio;
int32_t Macc = 500*micro_ratio;
int32_t Cacc = 500*micro_ratio;
int32_t ScrewSpeed = 8*micro_ratio;
int32_t *parameters[10] = {&Zzero_offset, &Mzero_offset, &Czero_offset,&Zspeed,&Mspeed,&Cspeed,&Zacc,&Macc,&Cacc,&ScrewSpeed};

int32_t value = 0;
int32_t disp_value = 0;
//Stepper Z
AccelStepper stepperZ(
 AccelStepper::DRIVER, //Motor type
 D3,                  //Step signal
 D14,                //Direction signal
 false);            //Motor enable on manual control

//Stepper C
AccelStepper stepperC(
 AccelStepper::DRIVER, //Motor type
 D4,                  //Step signal D8
 D14,                 //Direction signal
 false);            //Motor enable on manual control

//Stepper M
AccelStepper stepperM(
 AccelStepper::DRIVER, //Motor type
 D2,                  //Step signal
 D13,                 //Direction signal D14
 false);            //Motor enable on manual control

AccelStepper *ZSPoint = &stepperZ;    //
AccelStepper *CSPoint = &stepperC;    //
AccelStepper *MSPoint = &stepperM;    //


void setup() {
 
  stp1tour = ceil(motor_step_rot*Cgear*Ctrans*micro_ratio);  //number of step in a rotation of C axis
  pin_init();
  stepper_std();
  
  MSPoint->setCurrentPosition(Mzero_offset);
  ZSPoint->setCurrentPosition(Zzero_offset);
  
  //Sets the direction of X,Z,C,M the right way ************************************************
  stepperZ.setPinsInverted(true,true,true);
  stepperC.setPinsInverted(true,true,true);
  stepperM.setPinsInverted(false,true,true); 
  
  RPC.begin();
  RPC.bind("Ref",Refgo);
  RPC.bind("Get_flask",Get_flaskgo);
  RPC.bind("Align",Aligngo);
  RPC.bind("Unscrew",Unscrewgo);
  RPC.bind("reScrew",reScrewgo);
  RPC.bind("Bringback",Bringbackgo);
  RPC.bind("untigh_up",untigh_upgo);
  RPC.bind("isMVT",isMVT);
  RPC.bind("GetDown",Getdowngo);
  RPC.bind("Backto1",backto1);
  RPC.bind("RefM",goRefM);
  RPC.bind("GoToStandby",goToStandby);
  RPC.bind("Decap",goDecap);
  RPC.bind("Recap",goRecap);
  RPC.bind("setParams",goSetParams);
  RPC.bind("MoveZ",goMoveZ);
  RPC.bind("MoveM",goMoveM);
  RPC.bind("MoveC",goMoveC);
  RPC.bind("readM4Params",readM4Params);

  //TO BE REMOVED
  stepperC.setCurrentPosition(Czero_offset);
  stepperZ.setCurrentPosition(Zzero_offset);
  stepperM.setCurrentPosition(Mzero_offset);
}

void loop() {
int temp = 0;
bool res = true;
switch(status){
  case -1: //Initial state, before init
    //RPC.println("0");
    delay(100);
    break;

  case 0: //M4 get in this state after finishing a task
    //RPC.println("1");
    delay(100);
    break;

  case 1:
    delay(200);
    pin_init();
    Ref();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 2:
    //noInterrupts();
    pin_reinit();
    Get_flask();
    //interrupts();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;
  
  case 3:
    pin_reinit();
    Getdown();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 4:
    //RPC.println("4");
    //temp = *PC_pos;
    //RPC.println(temp);
    pin_reinit();
    Align();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 5:
    pin_reinit();
    *PZ_screw = Unscrew();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 6:
    pin_reinit();
    Bringback();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 7:
    pin_reinit();
    reScrew(Z_screw);
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;
  
  case 8:
    pin_reinit();
    untigh_up();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 9:
    pin_init();
    refM();
    stepper_std();
    MSPoint->moveTo(Mzero_offset);
    while(abs(MSPoint->distanceToGo())>0){stepperM.run();}
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;
    
  case 10:
    pin_reinit();
    ToStandby();delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;
    
  case 11:
    pin_reinit();
    Decap();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 12:
    pin_reinit();
    Recap();
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;
  case 13:
    RPC.flush();
    RPC.print("Parameters written :");
    for(int i = 0;i<10;i++){
      RPC.print(*parameters[i]/micro_ratio);RPC.print(" ");
    }
    RPC.println(" ");
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 14:
    MoveZ(disp_value);
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;

  case 15:
    MoveM(disp_value);
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;
  case 16:
    MoveC(disp_value);
    delay(1000);
    res = RPC.call("M4TaskCompleted").as<bool>();
    status = 0;
    break;
}

}
