#include <Portenta_Ethernet.h>
#include <Ethernet.h>

void write_parameters(EthernetClient client){
  char data[4] = {0,0,0,0};
  long params[10] = {0,0,0,0,0,0,0,0,0,0};
  int counter = 0;
  Serial.print(client.available());Serial.println(" : bytes available for reading");
  while(client.available()>3){
    for(int i = 0; i<4; i++){
      data[i] = client.read();
    }
    params[counter++] = chartolong(data);
  }
  
}

void read_parameters(EthernetClient client){
  client.write(Zstandby);
  client.write(Mstandby);
  client.write(Cstandby);
  
  client.write(Zspeed);
  client.write(Mspeed);
  client.write(Cspeed);
  
  client.write(Zacc);
  client.write(Macc);
  client.write(Cacc);

  client.write(ScrewSpeed);
}

long chartolong(char data[4]){
  long a = 0;
  a  = data[0] + data[1]<<8 + data[2] <<16 + data[3] <<24;
  return a;
}
