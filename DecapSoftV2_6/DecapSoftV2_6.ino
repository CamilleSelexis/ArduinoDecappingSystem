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
long Zstandby = 100000;
long Mstandby = 63000;
long Cstandby = 0;
long Zspeed = 5000;
long Mspeed = 6000;
long Cspeed = 6000;
long Zacc = 3500;
long Macc = 3500;
long Cacc = 3500;
long ScrewSpeed = 4;
long parameters[10] = {Zstandby, Mstandby, Cstandby,Zspeed,Mspeed,Cspeed,Zacc,Macc,Cacc,ScrewSpeed};
 //Ethernet related ---------------------
byte mac[] = {0xDE, 0xA1, 0x00, 0x73, 0x24, 0x12};  //Mac adress

//IPAddress ip(10,0,16,10);   //Adresse IP
IPAddress ip(192,168,0,2);
EthernetServer server = EthernetServer(52);  // (port 80 is default for HTTP) 52 is the number of the lab


//-------------------------------------------//
void setup(){
  bootM4();
  RPC1.begin();
  RPC1.bind("M4TaskCompleted",M4TaskCompleted);
  //RPC1.bind("setParams", setParams);
  Serial.begin(baud); //Begin serial communication aka discussion through usb
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
  digitalWrite(LEDB,HIGH);
  delay(100);
  
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
        else if(currentLine.endsWith("Initialize")){
          client.print("Initialize");
          refAllHome();
          } 
        else if(currentLine.endsWith("Decap")){
          client.print("Decap routine");
          Decap();
        }
        else if(currentLine.endsWith("Recap")){
          client.print("Recap routine");
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
          client.print("Alignement Routine");
          Align();        
          }
          else if(currentLine.endsWith("ReadParameters")){
            Serial.println("Sending params");
            //client.print("Sending params");
            read_parameters(client);   
          }
          else if(currentLine.endsWith("WriteParameters")){
            //write_parameters(client);
            *Pworking = true;
            char data[4] = {0,0,0,0};
            int32_t params[10] = {0,0,0,0,0,0,0,0,0,0};
            int counter = 0;
            delay(100);
            Serial.print(client.available());Serial.println(" : bytes available for reading");
            if(client.available() != 40)break;
            while(client.available()>3){
              for(int i = 0; i<4; i++){
                data[i] = client.read();
              }
              params[counter++] = (int32_t)chartolong(data);
            }
            for(int j=0;j<10;j++){
              Serial.print(params[j]);Serial.print(" ");
            }
            Serial.println("");
            Serial.println("Done writing parameters");
            client.print("Done writing parameters");
            client.flush();
            client.stop();

            for(int i = 0;i<10;i++){
              byte bytes[4] = {(params[i] >> 0) & 0xFF, (params[i] >> 8) & 0xFF, (params[i] >> 16) & 0xFF, (params[i] >> 24) & 0xFF};
              byte result = RPC1.call("setParams",i,bytes[0],bytes[1],bytes[2],bytes[3]).as<byte>(); //From M7 setting a variable in M4
              delay(100);
              Serial.println("Called the RPC function " + String(params[i]));
            }
            bool result = RPC1.call("readM4Params").as<bool>();
            //delay(1000);
            //RPC1.setTimeout(2000);
            while(*Pworking){
              if(RPC1.available()){
                Serial.write(RPC1.read());
              }
            }

          }
          else if(currentLine.endsWith("Manual control")){
            client.print("Going into manual control, use the serial console to control me");
            client.stop();
            //manualControl();
            while(1){
              digitalWrite(LEDR,LOW);
              delay(100);
              client = server.available();
              if (client) {
                  // an http request ends with a blank line
                  while (client.connected()) {
                    if(client.available()){
                      char data = client.read();
                      if(data == 'Z'){
                        data = client.read();
                        if(data == 'M'){
                          char value[4] = {0,0,0,0};//The value is a long
                          value[0] = client.read();value[1] = client.read();value[2] = client.read();value[3] = client.read();
                          long val = chartolong(value);
                          Serial.print("Received value for movement in X of : ");Serial.println(val);
                          Move('X',val);
                          
                        }
                        else if(data == 'S'){
                          char value[4] = {0,0,0,0};//The value is a long
                          value[0] = client.read();value[1] = client.read();value[2] = client.read();value[3] = client.read();
                          long val = chartolong(value);
                          Serial.print("Received value for speed in X of : ");Serial.println(val);
                          Speed('X',val);
                        }
                      }
                      if(data == 'C'){
                        data = client.read();
                        if(data == 'M'){
                          char value[4] = {0,0,0,0};//The value is a long
                          value[0] = client.read();value[1] = client.read();value[2] = client.read();value[3] = client.read();
                          long val = chartolong(value);
                          Serial.print("Received value for movement in X of : ");Serial.println(val);
                          Move('C',val);
                          
                        }
                        else if(data == 'S'){
                          char value[4] = {0,0,0,0};//The value is a long
                          value[0] = client.read();value[1] = client.read();value[2] = client.read();value[3] = client.read();
                          long val = chartolong(value);
                          Serial.print("Received value for speed in X of : ");Serial.println(val);
                          Speed('C',val);
                        }
                      }
                      if(data == 'M'){
                        data = client.read();
                        if(data == 'M'){
                          char value[4] = {0,0,0,0};//The value is a long
                          value[0] = client.read();value[1] = client.read();value[2] = client.read();value[3] = client.read();
                          long val = chartolong(value);
                          Serial.print("Received value for movement in X of : ");Serial.println(val);
                          Move('M',val);
                          
                        }
                        else if(data == 'S'){
                          char value[4] = {0,0,0,0};//The value is a long
                          value[0] = client.read();value[1] = client.read();value[2] = client.read();value[3] = client.read();
                          long val = chartolong(value);
                          Serial.print("Received value for speed in X of : ");Serial.println(val);
                          Speed('M',val);
                        }
                      }
                      if(data == 'Q'){
                        Serial.println("I will exit manual control");
                        client.print("I will exit manual control");
                      }
                      else{
                        client.print("Message must start by M,C or Z");
                      }
                      client.flush();
                      client.stop();
                    }
                  }
              }
              digitalWrite(LEDR,HIGH);
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
