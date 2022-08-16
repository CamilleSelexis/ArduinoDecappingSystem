void endConnection(EthernetClient* client_pntr){
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

//This function is called when the decapper can perform the asked action
void answerHttp(EthernetClient* client_pntr,String currentLine){
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println();
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

//This function is called when the call won't be performed
void answerHttpNo(EthernetClient* client_pntr,String currentLine, int state){
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println();
  client_pntr->print("The decapper cannot perform this action now, status=");client_pntr->println(state);
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

void homePage(EthernetClient* client_pntr){

  int state = getStatus();
  long current_time = millis();
  int seconds = (int) (current_time / 1000) % 60 ;
  int minutes = (int) ((current_time / (1000*60)) % 60);
  int hours   = (int) ((current_time / (1000*60*60)) % 24);
  char c[30];
  int l = sprintf(c, "%02d:%02d:%02d",hours,minutes,seconds);
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  client_pntr->println("Refresh: 10");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->println("<!DOCTYPE HTML>");
  client_pntr->println("<html>");
  client_pntr->println("<body>");
  client_pntr->print("<h1 style=\"text-align:center\">Decapper ");client_pntr->print(DECAP_NUMBER);client_pntr->println("</h1>");
  client_pntr->print("<p>Current Status : ");client_pntr->print(state);
  client_pntr->print("<img src=\"data:image/png;base64," + (state==3 ? yellow_dot:(state<2 ?green_dot:red_dot)) + "\" alt=\"Red dot\" />");
  client_pntr->println("</p>");
  client_pntr->print("<p>Time since last reset : " + String(c));client_pntr->println("</p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/initialize\">Initialize</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/decap\">Decap</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/recap\">Recap</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/capture\">Capture</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/decapperStatus\">Status</a></p>");
  client_pntr->println("<p><a href=\"http://" + StringIP + "/reset\">Reset</a></p>");
  client_pntr->println("</body>");
  client_pntr->println("</html>");
  
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
//This function returns the status of the decapper
//There is X possible status
//0 : decapper out of order
//1 : decapper currently working
//2 : decapper free (ready for decap)
//3 : decapper used (ready for recap)
//4 : decapper not initialised

void statusHttp(EthernetClient* client_pntr,String currentLine){
  int state = getStatus();
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->print("status=");client_pntr->println(state);

  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  delay(10);
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
