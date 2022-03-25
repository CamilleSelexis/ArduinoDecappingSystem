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

bool Aligngo(long Cpos) {
  *PC_pos = Cpos;
  *Pstatus = 4;
  return true;
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
bool goDecap(long Cpos) {
  *Pstatus = 11;
  return true;
}

bool goRecap() {
  *Pstatus = 12;
  return true;
}
