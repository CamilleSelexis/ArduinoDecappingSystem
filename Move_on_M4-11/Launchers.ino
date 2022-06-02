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

long setParams(int index, long param){
  parameters[index] = param;
  *Pstatus = 13;
  return param;
}
