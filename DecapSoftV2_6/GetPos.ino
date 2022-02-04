//Functions serving the detection algorythm

float movingAvg(float in[], int n, int x) {

  float mavg = 0;
  for(uint8_t i = x-n;i <=(x+n); i++){
    mavg += in[i];
  }
  mavg = mavg/(2*n+1);
  return mavg;
}


long GetPos(){

  uint8_t vect[ly+1];
  vect[0] = floor(ly*0.5);
  for(int i = 1;i<=ly;i++){
    vect[i] = vect[i-1] + pow(-1,i)*i;
  }

  float Result[10];
  uint8_t found = 0;
  uint8_t Pos[100];
  Result[0] = 0;
  float avg[ly];
  float mvt[ly-2*n];

  if (cam.grab(Pfb) == 0){
    Serial.println("Snapshot");    
    }else{Serial.println("Cam capture failed");
      return 0;    
  }
    // Cropavg -----------
      uint8_t k = 0;
      for(uint8_t i = cropy[0]; i<=cropy[1]; i++){
        for(uint8_t j = cropx[0]; j<=cropx[1]; j++){
              avg[k] += fb[i*320+j];
        }
        k++;
      }
      for(uint8_t i = 0; i<ly; i++){
        avg[i] /= lx;
      }
      //------------------
    for(uint8_t i = n; i<ly-n; i++){
        mvt[i-n] = movingAvg(avg,n,i);
      }    

    for(uint8_t i = 2*n; i<= ly-2*n-1; i++){
      bool diff1 = abs(mvt[vect[i]]-mvt[vect[i+2*n]]) > thres;
      bool diff2 = abs(mvt[vect[i]]-mvt[vect[i-2*n]]) > thres;

      if(diff1 && diff2) {
          Pos[found] = vect[i];
          found++;
        }
      }

    delay(10);
    Serial.print("Capture found ");
    Serial.print(found);
    Serial.println(" markers.");
    Pos[found] = 0;

    uint8_t i,j = 0;
     k = 0;

    while(j < found -1 && k <10){
      i = 1;
      Result[k] = Pos[j];
      while(abs(Pos[j]-Pos[j+1])<4 && j < found -1){
        Result[k] += Pos[j+1];
        i++;j++;
      }
      Result[k] /= i;
      k++;j++;
    }


    //Serial.println(Result[0]);

    float pixtomm = 0.16286; //Should be right
    float pi = 3.14159265358979323846;  //is definitely right

    Result[0] = (Result[0] + cropy[0] - 120) * pixtomm;

    return -ceil(stp1tour*(atan2(Result[0],24)/(2*pi)))+calibration;

}

//Calls GetPos() and check its reliability over 2 iterations
long finalPos() {
  cam.standby(false);

  uint8_t tol = 100;
  uint8_t i = 0;
  delay(50);

  long temp1 = GetPos();
  delay(50);
  long temp2 = GetPos();
  delay(50);
  while(abs(temp1-temp2)>tol && i<20){
    temp1 = GetPos();
    delay(50);
    temp2 = GetPos();
    delay(50);
    i++;
  }

  cam.standby(true);
  return ceil((temp1+temp2)*0.5);
}
