long bytetolong(byte data[4]){
  long a = 0;
  a  = data[0]|(data[1]<<8)|(data[2] <<16)|(data[3] <<24);
  return a;
}
int chartoint(char data[2]){
  int a = 0;
  a  = data[0]|(data[1]<<8);
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
