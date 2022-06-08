bool Refgo(){
  *Pstatus = 1;
  return true;
}

bool Get_flaskgo() {
  *Pstatus = 2;
  return true;
}

bool Getdowngo() {
  *Pstatus = 3;
  return true;
}

int Aligngo(int Cpos) {
  //int temp = Cpos;
  //RPC1.println(Cpos);
  //delay(100);
  *PC_pos = Cpos;
  *Pstatus = 4;
  return 1; //Cannot return a value longer than a bit?
}

bool Unscrewgo() {
  *Pstatus = 5;
  return true;
}

bool Bringbackgo() {
  *Pstatus = 6;
  return true;
}

bool reScrewgo() {
  *Pstatus = 7;
  return true;
}

bool untigh_upgo() {
  *Pstatus = 8;
  return true;
}

bool backto1() {
  *Pstatus = -1;
  return false;
}

bool goRefM() {
  *Pstatus = 9;
  return true;
}
bool goToStandby() {
  *Pstatus = 10;
  return true;
}
int goDecap(int Cpos) {
  *PC_pos = Cpos;
  *Pstatus = 11;
  return true;
}

bool goRecap() {
  *Pstatus = 12;
  return true;
}

byte goSetParams(byte index,byte bytes0,byte bytes1,byte bytes2,byte bytes3){
  byte data[4] = {bytes0,bytes1,bytes2,bytes3};
  int32_t param = bytetolong(data);
  *parameters[index] = param*micro_ratio;
  //*Pstatus = 13;
  return bytes0;
}
bool readM4Params(){
  *Pstatus = 13;
  return true;
}

byte goMoveZ(byte bytes0, byte bytes1, byte bytes2, byte bytes3){
  *Pstatus = 14;
  byte data[4] = {bytes0,bytes1,bytes2,bytes3};
  disp_value = bytetolong(data);
  RPC1.println(disp_value);
  return bytes0;
}

long goMoveM(byte bytes0, byte bytes1, byte bytes2, byte bytes3){
  *Pstatus = 15;
  byte data[4] = {bytes0,bytes1,bytes2,bytes3};
  disp_value = bytetolong(data);
  return bytes0;
}

long goMoveC(byte bytes0, byte bytes1, byte bytes2, byte bytes3){
  *Pstatus = 16;
  byte data[4] = {bytes0,bytes1,bytes2,bytes3};
  disp_value = bytetolong(data);
  return bytes0;
}
