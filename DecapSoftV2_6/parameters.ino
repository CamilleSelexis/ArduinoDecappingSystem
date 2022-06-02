#include <Portenta_Ethernet.h>
#include <Ethernet.h>

void write_parameters(EthernetClient client){
  char data[4] = {0,0,0,0};
  long params[10] = {0,0,0,0,0,0,0,0,0,0};
  int counter = 0;
  delay(100);
  Serial.print(client.available());Serial.println(" : bytes available for reading");
  while(client.available()>3){
    for(int i = 0; i<4; i++){
      data[i] = client.read();
    }
    params[counter++] = chartolong(data);
  }
  Serial.println("Done writing parameters");
}

void read_parameters(EthernetClient client){
  byte myByte[4];
  byte byteArray[40];
  //Zstandby
  for(int i = 0; i<10;i++){
      for(int j = 0;j<4;j++){
        byteArray[3-j+i*4] = (parameters[i]>>j*8) & 0xFF; //Fill 3, 2, 1, 0, 7, 6, 5, 4, etc...
      }
      Serial.print(byteArray[i*4 + 0],HEX);Serial.print(byteArray[i*4+1],HEX);
      Serial.print(byteArray[i*4+2],HEX);Serial.println(byteArray[i*4+3],HEX);
  }
  client.write(byteArray,40);
}

long chartolong(char data[4]){
  long a = 0;
  a  = data[0]|(data[1]<<8)|(data[2] <<16)|(data[3] <<24);
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

long setParams(int index, long param){
  parameters[index] = param;
  return param;
}
