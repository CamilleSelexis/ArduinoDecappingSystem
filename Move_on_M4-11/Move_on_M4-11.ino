/*
Move_on_M4 v6 by Jérémie Pochon
Selexis Genève 20.08.2021
To run on an Arduino Portenta H7 with a vision shield
M4 core
In coordination with DecapSoft on M7 core
*/

#include <AccelStepper.h>
#include "Arduino.h"
#include "RPC_internal.h"
using namespace rtos;

#define mvt_in  digitalWrite(LEDG,LOFF);digitalWrite(LEDR,LON)
#define mvt_out digitalWrite(LEDR,LOFF);digitalWrite(LEDG,LON)

#define runCZ    stepperC.run();stepperZ.run()
//#define runMZ    stepperM.run();stepperZ.run()
//#define runXC    stepperX.run();stepperC.run()
#define runMC    stepperM.run();stepperC.run()
#define runXZ    stepperX.run();stepperZ.run()
#define runXMZ   stepperX.run();stepperM.run();stepperZ.run()
#define runXCZ   stepperX.run();stepperC.run();stepperZ.run()
#define runXCM   stepperX.run();stepperC.run();stepperM.run()
#define runALL   stepperX.run();stepperC.run();stepperM.run();stepperZ.run()
#define stopALL  stepperX.stop();stepperC.stop();stepperM.stop();stepperZ.stop();
#define pin_contact A2 //Used on M
#define pin_photo   A0 //Photodetecteur C

#define pin_BX      D6
#define pin_BZ      D0
#define DIR_MX      D13
#define DIR_CZ      D14

const int LON = LOW; // Voltage level is inverted for the LED
const int LOFF = HIGH;
const long motor_step_rot = 200;
const long micro_ratio = 2;
const long screw_time = 10; //Time to screw/unscrew [s]
const long X_pos = 65300;   //Position of the basket under the arm
//const long X_pos = 64800;   //Position of the basket under the arm
const long Xzero_offset = 10000; //Standby position X 
const uint Xgear = 5;        //X motor gearbox

const long Zzero_offset = 200000; //Standby position Z [steps]
const long Z_inter = Zzero_offset+130000; //Intermediate position
const long Z_pos = 462000;   //Position of the claws at the cap level
//const long Z_pos = 464000;   //Position of the claws at the cap level
const long Z_prec = 2000;   //Precontrainte on the spring
const long Zgear = 19;        //Z motor gearbox


const long Czero_offset = 0;    //Standby position C [tours]
const long Cgear = 139;         //C motor gearbox
const float Ctrans = 1.25;       //C secondary gear ratio
long stp1tour;       //Number of step in one C rotation.

const uint pasLBW = 8;     //Pas de vis du Labware

const long Mzero_offset = 126000; //Standby position M
const uint Mgear = 100;       //M motor gearbox
const long M_ser = 106000;         //Active position M

volatile int8_t status = -1;
volatile int8_t* Pstatus = &status;

long C_pos = 0;
long *PC_pos = &C_pos;

long C_start;
long* PC_start = &C_start;

//-----Stepper Definition------//
//Stepper X
AccelStepper stepperX(
 AccelStepper::DRIVER,   //Motor type
 D5,                    //Step signal
 D13,                   //Direction signal
 false);              //Motor enable on manual control

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

AccelStepper *XSPoint = &stepperX;    //Pointers to the accelsteppers objects
AccelStepper *ZSPoint = &stepperZ;    //
AccelStepper *CSPoint = &stepperC;    //
AccelStepper *MSPoint = &stepperM;    //


void setup() {
 
  stp1tour = ceil(motor_step_rot*Cgear*Ctrans*micro_ratio);  //number of step in a rotation of C axis
  pin_init();
  stepper_std();
  
  MSPoint->setCurrentPosition(Mzero_offset);
  XSPoint->setCurrentPosition(Xzero_offset);
  ZSPoint->setCurrentPosition(Zzero_offset);
  
  //Sets the direction of X,Z,C,M the right way ************************************************
  stepperX.setPinsInverted(true,true,true);
  stepperZ.setPinsInverted(false,true,true);
  stepperC.setPinsInverted(false,true,true);
  stepperM.setPinsInverted(true,true,true); 
  
  RPC1.begin();
  RPC1.bind("Ref",Refgo);
  RPC1.bind("Get_flask",Get_flaskgo);
  RPC1.bind("Align",Aligngo);
  RPC1.bind("Unscrew",Unscrewgo);
  RPC1.bind("reScrew",reScrewgo);
  RPC1.bind("Bringback",Bringbackgo);
  RPC1.bind("untigh_up",untigh_upgo);
  RPC1.bind("isMVT",isMVT);
  RPC1.bind("GetDown",Getdowngo);
  RPC1.bind("Backto1",backto1);
  RPC1.bind("RefM",goRefM);
  
}

void loop() {

switch(status){
  case -1:
    RPC1.println("0");
    delay(100);
    break;

  case 0:
    RPC1.println("1");
    delay(100);
    break;

  case 1:
    pin_init();
    Ref();
    status = 0;
    break;

  case 2:
    noInterrupts();
    pin_reinit();
    Get_flask();
    interrupts();
    status = 0;
    break;
  
  case 3:
    noInterrupts();
    pin_reinit();
    Getdown();
    interrupts();
    status = 0;
    break;

  case 4:
    noInterrupts();
    pin_reinit();
    Align(C_pos);
    interrupts();
    status = 0;
    break;

  case 5:
    noInterrupts();
    pin_reinit();
    *PC_start = Unscrew();
    interrupts();
    status = 0;
    break;

  case 6:
    noInterrupts();
    pin_reinit();
    Bringback();
    interrupts();
    status = 0;
    break;

  case 7:
    noInterrupts();
    pin_reinit();
    reScrew(C_start);
    interrupts();
    status = 0;
    break;
  
  case 8:
    noInterrupts();
    pin_reinit();
    untigh_up();
    interrupts();
    status = 0;
    break;

  case 9:
    pin_init();
    refM();
    stepper_std();
    MSPoint->moveTo(Mzero_offset);
    while(abs(MSPoint->distanceToGo())>0){stepperM.run();}
    status = 0;
    break;
}

}
