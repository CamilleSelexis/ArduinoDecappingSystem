#include <Portenta_Ethernet.h>
#include <Ethernet.h>

void write_parameters(EthernetClient* client_pntr){
  *Pworking = true;
  char data[4] = {0,0,0,0};
  int32_t params[10] = {0,0,0,0,0,0,0,0,0,0};
  int counter = 0;
  delay(100);
  Serial.print(client_pntr->available());Serial.println(" : bytes available for reading");
  if(client_pntr->available() != 40) return;
  while(client_pntr->available()>3){
    for(int i = 0; i<4; i++){
      data[i] = client_pntr->read();
    }
    params[counter++] = (int32_t)chartolong(data);
  }
  for(int j=0;j<10;j++){
    Serial.print(params[j]);Serial.print(" ");
  }
  Serial.println("");
  Serial.println("Done writing parameters");
  client_pntr->print("Done writing parameters");

  for(int i = 0;i<10;i++){
    *parameters[i] = params[i];
    byte bytes[4] = {(params[i] >> 0) & 0xFF, (params[i] >> 8) & 0xFF, (params[i] >> 16) & 0xFF, (params[i] >> 24) & 0xFF};
    byte result = RPC.call("setParams",i,bytes[0],bytes[1],bytes[2],bytes[3]).as<byte>(); //From M7 setting a variable in M4
    delay(100);
    Serial.println("Called the RPC function " + String(params[i]));
  }
  bool result = RPC.call("readM4Params").as<bool>();
  while(*Pworking){
    if(RPC.available()){
      client_pntr->write(RPC.read());
    }
  }
  client_pntr->flush();
  client_pntr->stop();
}

void read_parameters(EthernetClient client){
  byte myByte[4];
  byte byteArray[40];
  for(int i = 0; i<10;i++){
      for(int j = 0;j<4;j++){
        byteArray[3-j+i*4] = (*parameters[i]>>j*8) & 0xFF; //Fill 3, 2, 1, 0, 7, 6, 5, 4, etc...
      }
//      Serial.print(byteArray[i*4 + 0],HEX);Serial.print(byteArray[i*4+1],HEX);
//      Serial.print(byteArray[i*4+2],HEX);Serial.println(byteArray[i*4+3],HEX);
  }
  client.write(byteArray,40);
}

long chartolong(char data[4]){
  long a = 0;
  a  = data[0]|(data[1]<<8)|(data[2] <<16)|(data[3] <<24);
  Serial.println(a);
  return a;
}
int32_t bytetolong(byte data[4]){
  int32_t a = 0;
  a  = data[0]|(data[1]<<8)|(data[2] <<16)|(data[3] <<24);
  Serial.println(a);
  return a;
}
int chartoint(char data[2]){
  int a = 0;
  a  = data[0]|(data[1]<<8);
  Serial.println(a);
  return a;
}
byte* longtochar(long mylong){
  static byte myByte[4];
  myByte[3] = mylong & 0xFF;
  myByte[2] = (mylong>>8) & 0xFF;
  myByte[1] = (mylong>>16) & 0xFF;
  myByte[0] = (mylong>>24) & 0xFF;
  return myByte;
}

bool M4TaskCompleted(){
  *Pworking = false;
  return true;
}
