/*
Decapeur Software V2.5 by Jérémie Pochon & Camille Aussems
Selexis Genève 17.01.2022
To run on an Arduino Portenta H7 with a vision shield
M7 core
In coordination with Move_on_M4 on M4 core
Vision not yet verified
*/

#include <Portenta_Ethernet.h>
#include <Ethernet.h>
#include <camera.h>
#include <math.h>
#include <stdio.h>

#include "Arduino.h"
#include "RPC_internal.h"
using namespace rtos;

#include <stdint.h>

#define mvt_in  digitalWrite(LEDG,LOFF);digitalWrite(LEDR,LON)
#define mvt_out digitalWrite(LEDR,LOFF);digitalWrite(LEDG,LON)

#define motorON   digitalWrite(D9,HIGH);delay(1)
#define motorOFF  digitalWrite(D9,LOW)

#define pin_crydom  D7 //Relay

const int LON = LOW; // Voltage level is inverted for the LED
const int LOFF = HIGH;

const int Cgear = 139;         //C motor gearbox
const float Ctrans = 1.25;       //C secondary gear ratio
const int Cmicrosteps = 4;     //Microsteps
long stp1tour;       //Number of step in one C rotation.

bool isInit = false;          //Est-ce que la machine est initialisée
bool *PisInit = &isInit;      //Est uniquement initialisée false, puis est passée en true par la routine init.

bool C_start=false;            //Start of the C rotation when tightening
bool* CstartPoint = &C_start; //Pointer to Cstart

volatile bool M4work = false;          //Cette variable est vrai lorsque le M4 effectue une tache
volatile bool *Pworking = &M4work;     //N'est pas utilisé pour le moment mais pourrait être utile

int baud = 115200;          //Baud rate of the serial comunication

CameraClass cam;
uint8_t fb[320*240];        //Buffer for the image capture
uint8_t *Pfb = fb; 
const int imgH = 240;
const int imgW = 320;
//const uint8_t cropx[2] = {120,140};    //Si ze of the cropped image
//const uint8_t cropy[2] = {110,180};
const int cropx[2] = {120,140};    //Si ze of the cropped image
const int cropy[2] = {100,200};
const int ly = cropy[1]-cropy[0]; //Length of the cropped dimmensions
const int lx = cropx[1]-cropx[0];

//Light depending parameters for the image detection (will probably need a tweek for each environement)
//Try to change thres and n to have the minimum amount of markers detected while never having 0 of them.
const uint8_t thres = 8;         //Threshold of image detection 2->10
const uint8_t n = 2;             //Size of the moving average avg done on 2n+1

//Calibration should be determined by the standby position
const long calibration = 500;    //Rotation offset

 //Ethernet related ---------------------
byte mac[] = {0xDE, 0xA1, 0x00, 0x73, 0x24, 0x12};  //Mac adress

IPAddress ip(10,0,16,10);   //Adresse IP

EthernetServer server = EthernetServer(52);  // (port 80 is default for HTTP) 52 is the number of the lab


//-------------------------------------------//
void setup(){
  bootM4();
  RPC1.begin(); 

  Serial.begin(baud); //Begin serial communication aka discussion through usb
  while(!Serial);
  Serial.println("Serial Coms started. RPC1 starting...");
  pin_init();       //Initialise the pin modes, initial values and interrupts
  digitalWrite(LEDB,LON);

  stp1tour = ceil(200*Cgear*Ctrans*Cmicrosteps);  //number of step in a rotation of C axis: 34750

  if(cam.begin(CAMERA_R320x240, 30)== 0){
    Serial.println("Cam initialised");//initialise the camera
  }
  else{
    Serial.println("Cam failed to initialize");
  }
  //cam.standby(true);                //Put it in standby mode
                 //Initialise the RPC1 coms, also boots the M4
  
  Serial.println("Ethernet Coms starting...");
  
  Ethernet.begin(mac,ip);  //Start the Ethernet coms

// Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }


// Start the server
  server.begin();           //"server" is the name of the object for comunication through ethernet
  Serial.print("Ethernet server connected. Server is at ");
  Serial.println(Ethernet.localIP());         //Gives the local IP through serial com

  digitalWrite(LEDB,LOFF);
  digitalWrite(LEDG,LON);      //Green Led while available

} //End of setup loop


//---------------------LOOP---------------------//

void loop() {

//No need to change that, just remembers it seek the string at the end of a line. So "blabla" will also trigger "anyblabla"
  
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    String currentLine = "";
    while (client.connected()) {
      if(client.available()){
        char c = client.read();
        
        if(c == '\n'){
          if(currentLine.length() !=0){
            Serial.println(currentLine);
            currentLine = "";}
        } else if(c != '\r'){
          currentLine += c;
        }
        
        
        if(currentLine.endsWith("Status")){
          Status();
        } 
        else if(currentLine.endsWith("Init")){
          refAllHome();
          } 
        else if(currentLine.endsWith("Decap")){
          Decap();
        }
        else if(currentLine.endsWith("Recap")){
          Recap();
        }  
        else if(currentLine.endsWith("Sudo Uncap")){
          SudoDecap();
          } 
        else if(currentLine.endsWith("Sudo Ricap")){
          SudoRecap();
          }
        else if(currentLine.endsWith("Standby")){
          GoToStandby();
        }
        else if(currentLine.endsWith("capture")){
          Serial.println("Capture Routine");
          finalPos();        
          }
        else if(currentLine.endsWith("Align")){
          Serial.println("Alignment Routine");
          Align();        
          }      
          } 
      }     
client.stop(); 
  } 
}
