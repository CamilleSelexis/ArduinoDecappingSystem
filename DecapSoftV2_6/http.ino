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

  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->println("<!DOCTYPE HTML>");
  client_pntr->println("<html>");
  client_pntr->println("<body>");
  client_pntr->print("<h1 style=\"text-align:center\">Decapper ");client_pntr->print(DECAP_NUMBER);client_pntr->println("</h1>");
  client_pntr->print("<p>Current Status : ");client_pntr->print(M4work?"Busy":"Available");client_pntr->println("</p>");
  client_pntr->print("<p>Internal Time : ");client_pntr->print(millis());client_pntr->println("</p>");
  client_pntr->println("<p><a href=\"http://192.168.0.101/initialize\">Initialize</a></p>");
  client_pntr->println("<p><a href=\"http://192.168.0.101/decap\">Decap</a></p>");
  client_pntr->println("<p><a href=\"http://192.168.0.101/recap\">Recap</a></p>");
  client_pntr->println("<p><a href=\"http://192.168.0.101/capture\">Capture</a></p>");
  client_pntr->println("<p><a href=\"http://192.168.0.101/reset\">Reset</a></p>");
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
