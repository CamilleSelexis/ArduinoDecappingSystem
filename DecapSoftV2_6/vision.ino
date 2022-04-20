//Functions serving the detection algorythm

//Calls GetPos() and check its reliability over 2 iterations
long finalPos() {
  //cam.standby(false);
//    while(Serial.read() != '1'){};
//     Serial.write(fb, 320*240);
  uint8_t tol = 20;
  uint8_t i = 0;
  delay(50);

  long temp1 = detectEdges();
  delay(50);
  long temp2 = detectEdges();
  delay(50);
  while(abs(temp1-temp2)>tol && i<20){
    temp1 = detectEdges();
    delay(50);
    temp2 = detectEdges();
    delay(50);
    i++;
  }

  //cam.standby(true);

  float pixtomm = 0.16286; //Should be right
  float pi = 3.14159265358979323846;  //is definitely right
  //24 is the size of the cap
  temp1 = (temp1-imgH/2) * pixtomm;
  temp1 = -ceil(stp1tour*(atan2(temp1,24)/(2*pi)))+calibration;
  temp2 = (temp2-imgH/2) * pixtomm;
  temp2 = -ceil(stp1tour*(atan2(temp2,24)/(2*pi)))+calibration;
  Serial.print("Motor should move ");Serial.print(ceil((temp1+temp2)/2));
  Serial.println(" steps to align its claws");
  return ceil((temp1+temp2)*0.5);
}

long detectEdges() {

  float cropped2D[lx][ly];
  float filter2D[3][3] = {{-4,0,4},{-4,0,4},{-4,0,4}};
  float result2D[lx][ly];
  float gaussian2D[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
  float lineAvg[ly];
  float line[ly];
  int edge_pos[100]; //Max size if every single pixel is a max (impossible)
  if (cam.grab(fb) == 0){
    Serial.println("Capture done");
  }
  else{Serial.println("Couldn't take a capture");}
  for(int i = 0; i<lx; i++){ //Crop the framebuffer and put it in 2D
    for(int j = 0; j<ly;j++) {
      cropped2D[i][j] = 0;
      cropped2D[i][j] = fb[(j+cropy[0])*imgW+(i+cropx[0])];
      result2D[i][j] = 0;
    }
  }
  //Compute gaussian avg on the picture with a convolution
  convolution_2D(cropped2D,gaussian2D,result2D);
  for(int i = 0; i<lx; i++){
    for(int j = 0; j<ly; j++){
        cropped2D[i][j] = result2D[i][j]/16;
        result2D[i][j] = 0;
    }
  }
  //Use sobel filter on the averaged picture to detect the edges
  convolution_2D(cropped2D,filter2D,result2D);
  for(int i = 0; i<lx; i++){
    for(int j = 0; j<ly; j++){
        result2D[i][j] = abs(result2D[i][j]);
    }
  }
  //Compute avg on each lines
  for(int i=0;i<ly;i++){
    line[i] = 0;
    lineAvg[i] = 0;
    for(int j = 0; j<lx;j++){
      line[i] = line[i] + result2D[j][i]; //Compute sum on each line
    }
    line[i] = line[i]/lx; // Avg each line
  }
  movingAverage(line,lineAvg,2,ly);
  //Intensity Thresholding
  int int_thresh = 35; //Could add a smart way to compute the threshold by doing histograms
  for(int i = 0; i<ly; i++){
      if(lineAvg[i]> int_thresh)
          lineAvg[i] = 255;
      else
          lineAvg[i] = 0;
      line[i] = lineAvg[i];
  }
  movingAverage(line,lineAvg,1,ly);
  int k = 0;
  int found = 0;
  for(int i = 10; i<ly-10;i++){ // don't consider the 10 pixel on the borders
    if(lineAvg[i]>int_thresh && lineAvg[i]>=lineAvg[i-1] && lineAvg[i] >= lineAvg[i+1]){
      edge_pos[k++] = i;
      found++;
      Serial.print("Found max at : "); Serial.println(i);
    }
  }
  Serial.print("Found : ");Serial.print(found);Serial.println(" edges");
  long edge_position = 0;
  for(int i= 0; i<found; i++){
    if(abs(edge_pos[i]-ly/2) < abs(edge_position-ly/2)) edge_position = edge_pos[i];
  }
  Serial.print("Edge at position ");Serial.print(edge_position+ cropy[0]);Serial.println(" in the full picture");
  return edge_position + cropy[0];
  
}

//Takes 2 vectors in & out and put the moving average (2*n +1) in out
void movingAverage(float in[], float out[], int n, int l) {
  for(int i = n;i<l-n;i++){
    out[i] = 0;
    for(int j = i-n;j<=i+n;j++){
      out[i] = out[i] + in[j];
    }
    out[i] = out[i]/(2*n+1);
  }
}
//Perform a 2D convolution P = N*M where N is the original matrix, M a mask and P the result
void convolution_2D(float N[lx][ly], float M[3][3], float P[lx][ly]) {

// find center position of kernel (half of kernel size)

  for (int i = 0; i < lx; i++)              // rows
  {
      for (int j = 0; j < ly; j++)          // columns
      {
          for (int m = 0; m < 3; m++)     // kernel rows
          {
              for (int n = 0; n < 3; n++) // kernel columns
              {
                  // index of input signal, used to check boundary
                  int ii = i + (m - 1);
                  int jj = j + (n - 1);
  
                  // ignore input samples which are out of bound
                  if (ii >= 0 && ii < lx && jj >= 0 && jj < ly)
                    P[i][j] = P[i][j] + N[ii][jj] * M[m][n];
              }
          }
          //P[i][j] = abs(P[i][j]);
      }
  }
}
