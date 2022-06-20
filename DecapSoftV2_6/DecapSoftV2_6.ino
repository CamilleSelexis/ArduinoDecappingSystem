/*
Decap software v2.6
mbed-core v3.0
Developped by J. Pochon & C. Aussems
Upload on portenta H7 M7 core
Upload Move on M4 on portenta H7 M4 core
*/

#include <Portenta_Ethernet.h>
#include <Ethernet.h>
#include <math.h>
#include <stdio.h>

#include "Arduino.h"
#include "RPC.h"
#include "camera.h"
#include "himax.h"
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
HM01B0 himax;
Camera cam(himax);
#define IMAGE_MODE CAMERA_GRAYSCALE
FrameBuffer FB(320,240,1);
uint8_t fb[320*240];        //Buffer for the image capture
uint8_t *Pfb = fb; 
const int imgH = 240; //X dimension
const int imgW = 320; //Y dimension
const int cropx[2] = {120,140};    //Size of the cropped image
const int cropy[2] = {100,200};
const int ly = cropy[1]-cropy[0]; //Length of the cropped dimmensions
const int lx = cropx[1]-cropx[0];

//Light depending parameters for the image detection (will probably need a tweek for each environement)
//Try to change thres and n to have the minimum amount of markers detected while never having 0 of them.
const uint8_t thres = 8;         //Threshold of image detection 2->10
const uint8_t n = 2;             //Size of the moving average avg done on 2n+1

//Calibration should be determined by the standby position
const long calibration = 1000*Cmicrosteps;    //Rotation offset - 9267 is the distance between the bumps
const int cal_prop = 28; //Factor for the calibration based on the dist from the center
int32_t Zstandby = 140000;
int32_t Mstandby = 63000;
int32_t Cstandby = 1;
int32_t Zspeed = 1000;
int32_t Mspeed = 1000;
int32_t Cspeed = 1000;
int32_t Zacc = 500;
int32_t Macc = 500;
int32_t Cacc = 500;
int32_t ScrewSpeed = 8;
int32_t* parameters[10] = {&Zstandby, &Mstandby, &Cstandby,&Zspeed,&Mspeed,&Cspeed,&Zacc,&Macc,&Cacc,&ScrewSpeed};
 //Ethernet related ---------------------
byte mac[] = {0xDE, 0xA1, 0x00, 0x73, 0x24, 0x12};  //Mac adress

//IPAddress ip(10,0,16,10);   //Adresse IP
IPAddress ip(192,168,0,2);
EthernetServer server = EthernetServer(52);  // (port 80 is default for HTTP) 52 is the number of the lab


//-------------------------------------------//
void setup(){
  bootM4();
  RPC.begin();
  RPC.bind("M4TaskCompleted",M4TaskCompleted);
  //RPC.bind("setParams", setParams);
  Serial.begin(baud); //Begin serial communication aka discussion through usb
  Serial.println("Serial Coms started. RPC starting...");
  pin_init();       //Initialise the pin modes, initial values and interrupts
  digitalWrite(LEDB,LON);
  
  stp1tour = ceil(200*Cgear*Ctrans*Cmicrosteps);  //number of step in a rotation of C axis: 34750

  if(cam.begin(CAMERA_R320x240, IMAGE_MODE, 30)){
    Serial.println("Cam initialised");//initialise the camera
  }
  else{
    Serial.println("Cam failed to initialize");
  }
  //cam.standby(true);                //Put it in standby mode
                 //Initialise the RPC coms, also boots the M4
  
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
  digitalWrite(LEDB,HIGH);
  delay(100);
  
  // listen for incoming clients
  EthernetClient client = server.available();
  EthernetClient* client_pntr = &client;
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
          client.stop();
        } 
        else if(currentLine.endsWith("Initialize")){
          client.print("Initialize");
          refAllHome();
          client.stop();
          } 
        else if(currentLine.endsWith("Decap")){
          client.print("Decap routine");
          Decap();
          client.stop();
        }
        else if(currentLine.endsWith("Recap")){
          client.print("Recap routine");
          Recap();
          client.stop();
        }  
        else if(currentLine.endsWith("Sudo Uncap")){
          SudoDecap();
          client.stop();
          } 
        else if(currentLine.endsWith("Sudo Ricap")){
          SudoRecap();
          client.stop();
          }
        else if(currentLine.endsWith("Standby")){
          GoToStandby();
          client.stop();
        }
        else if(currentLine.endsWith("capture")){
          Serial.println("Capture Routine");
          finalPos();
          client.stop();
                  
          }
        else if(currentLine.endsWith("Align")){
          Serial.println("Alignment Routine");
          client.print("Alignement Routine");
          Align(); 
          client.stop();       
          }
          else if(currentLine.endsWith("ReadParameters")){
            Serial.println("Sending params");
            client.print("Sending params");
            read_parameters(client);
            client.stop(); 
          }
          else if(currentLine.endsWith("WriteParameters")){
            write_parameters(client_pntr);
            client.stop();

          }
          else if(currentLine.endsWith("Manual control")){
            Serial.println("Going into manual control");
            client.print("Going into manual control");
            client.stop();
            digitalWrite(LEDG,HIGH);
            digitalWrite(LEDB,HIGH);
            delay(1000);
            //manualControl();
            bool manual_ctrl = true;
            while(manual_ctrl){
              digitalWrite(LEDG,LOW);
              delay(100);
              EthernetClient client_manual = server.available();
              if (client_manual) {
                  // an http request ends with a blank line
                  while (client_manual.connected()) {
                    if(client_manual.available()){
                      char data = client_manual.read();
                      switch(data){
                        case 'Z':
                          data = client_manual.read();
                          if(data == 'M'){
                            delay(50);
                            byte value[4] = {0,0,0,0};//The value is a long
                            value[0] = (byte)client_manual.read();value[1] = (byte)client_manual.read();
                            value[2] = (byte)client_manual.read();value[3] = (byte)client_manual.read();
                            int32_t val = bytetolong(value);
                            Serial.print("Received value for movement in Z of : ");Serial.println(val);
                            *Pworking = true;
                            motorON;
                            Move('Z',value[0],value[1],value[2],value[3]);
                            while(*Pworking){
                              if(RPC.available()){
                                client_manual.write(RPC.read());
                              }
                            }
                            motorOFF;
                          }
                          else if(data == 'S'){
                            byte value[4] = {0,0,0,0};//The value is a long
                            value[0] = (byte)client_manual.read();value[1] = (byte)client_manual.read();
                            value[2] = (byte)client_manual.read();value[3] = (byte)client_manual.read();
                            int32_t val = bytetolong(value);
                            Serial.print("Received value for speed in Z of : ");Serial.println(val);
                            *Pworking = true;
                            Speed('Z',value[0],value[1],value[2],value[3]);
                            while(*Pworking){
                              if(RPC.available()){
                                client_manual.write(RPC.read());
                              }
                            }
                          }
                          break;
                      case 'M':
                        data = client_manual.read();
                        if(data == 'M'){
                          byte value[4] = {0,0,0,0};//The value is a long
                          value[0] = (byte)client_manual.read();value[1] = (byte)client_manual.read();
                          value[2] = (byte)client_manual.read();value[3] = (byte)client_manual.read();
                          int32_t val = bytetolong(value);
                          Serial.print("Received value for movement in M of : ");Serial.println(val);
                          *Pworking = true;
                          motorON;
                          Move('M',value[0],value[1],value[2],value[3]);
                          while(*Pworking){
                            if(RPC.available()){
                              client_manual.write(RPC.read());
                            }
                          }
                          motorOFF;
                        }
                        else if(data == 'S'){
                          byte value[4] = {0,0,0,0};//The value is a long
                          value[0] = (byte)client_manual.read();value[1] = (byte)client_manual.read();
                          value[2] = (byte)client_manual.read();value[3] = (byte)client_manual.read();
                          int32_t val = bytetolong(value);
                          Serial.print("Received value for speed in M of : ");Serial.println(val);
                          *Pworking = true;
                          Speed('M',value[0],value[1],value[2],value[3]);
                          while(*Pworking){
                            if(RPC.available()){
                              client_manual.write(RPC.read());
                            }
                          }
                        }
                        break;

                        case 'C':
                          data = client_manual.read();
                          if(data == 'M'){
                            byte value[4] = {0,0,0,0};//The value is a long
                            value[0] = (byte)client_manual.read();value[1] = (byte)client_manual.read();
                            value[2] = (byte)client_manual.read();value[3] = (byte)client_manual.read();
                            int32_t val = bytetolong(value);
                            Serial.print("Received value for movement in C of : ");Serial.println(val);
                            *Pworking = true;
                            motorON;
                            Move('C',value[0],value[1],value[2],value[3]);
                            while(*Pworking){
                              if(RPC.available()){
                                client_manual.write(RPC.read());
                              }
                            }
                            motorOFF;
                          }
                          else if(data == 'S'){
                            byte value[4] = {0,0,0,0};//The value is a long
                            value[0] = (byte)client_manual.read();value[1] = (byte)client_manual.read();
                            value[2] = (byte)client_manual.read();value[3] = (byte)client_manual.read();
                            int32_t val = bytetolong(value);
                            Serial.print("Received value for speed in C of : ");Serial.println(val);
                            *Pworking = true;
                            Speed('C',value[0],value[1],value[2],value[3]);
                            while(*Pworking){
                              if(RPC.available()){
                                client_manual.write(RPC.read());
                              }
                            }
                          }
                        break;
                        case 'Q':
                          Serial.println("I will exit manual control");
                          client_manual.print("I will exit manual control");
                          manual_ctrl = false;
                          break;
                        default:
                          client_manual.print("Message must start by M,C or Z");
                          break;
                      }
                      client_manual.flush();
                      client_manual.stop();
                    }
                  }
              }
              digitalWrite(LEDG,HIGH);
              delay(100);
            }
          }
 
          } 
      }
      client.stop();     
  }
  digitalWrite(LEDB,LOW);
  delay(100);
}
