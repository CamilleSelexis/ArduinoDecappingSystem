#include <Stepper.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Commander.h>

//#include <stdint.h>
//using namespace rtos;

#define mvt_in  digitalWrite(LEDG,HIGH);digitalWrite(LEDR,LOW)
#define mvt_out digitalWrite(LEDR,HIGH);digitalWrite(LEDG,LOW)
#define runXCM   stepperX.run();stepperC.run();stepperM.run()
#define runCZ    stepperC.run();stepperZ.run()
#define runMZ    stepperM.run();stepperZ.run()
#define runALL   stepperM.run();stepperZ.run();stepperX.run();stepperC.run()
#define stopALL  stepperM.stop();stepperZ.stop();stepperX.stop();stepperC.stop()
#define motorON   digitalWrite(D8,HIGH);digitalWrite(D9,HIGH)
#define motorOFF  digitalWrite(D8,LOW);digitalWrite(D9,LOW)

int flagX = 0;
int flagZ = 0;
Commander comder;

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
 D4,                  //Step signal
 D14,                 //Direction signal
 false);            //Motor enable on manual control

//Stepper M
AccelStepper stepperM(
 AccelStepper::DRIVER, //Motor type
 D2,                  //Step signal
 D13,                 //Direction signal
 false);            //Motor enable on manual control
 
AccelStepper *Xpointer = &stepperX;
AccelStepper *Zpointer = &stepperZ;
AccelStepper *Cpointer = &stepperC;
AccelStepper *Mpointer = &stepperM;

int commandVal;

//Now the sketch setup, loop and any other functions
void setup() {

  pin_init();
  stepper_init();
  //Sets the direction of X,Z,C,M the right way ************************************************
  stepperX.setPinsInverted(true,true,true);
  stepperZ.setPinsInverted(false,true,true);
  stepperC.setPinsInverted(true,true,true);
  stepperM.setPinsInverted(true,true,true);

  Serial.begin(115200);
  while(!Serial){;}                               //Wait for the serial port to open (if using USB)
  initialiseCommander();
  comder.commandPrompt(ON);                          //enable the command prompt
  comder.echo(true);                                 //Echo incoming characters to theoutput port
  Serial.println("Hello: Type 'help' to get help, base value set to 1000");
  commandVal = 1000;
  comder.printCommandPrompt();
  digitalWrite(LEDG,LOW);                         //Enable green LED to show that the arduino is ready
}

void loop() {
  // put your main code here, to run repeatedly:
  comder.update();
  runALL;
  if(digitalRead(A0)==HIGH){
    Serial.println("Detector C touched");
  }
    if(flagX == 1){
    Serial.println("Detector X touched");
    flagX = 0;
  }
    if(flagZ == 1){
    Serial.println("Detector Z touched");
    flagZ = 0;
  }
}
