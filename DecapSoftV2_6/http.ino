void endConnection(EthernetClient* client_pntr){
  // send a standard http response header
  /*client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();*/
  client_pntr->print("Connection closed by the server at internal time : ");client_pntr->println(millis());
  /*client_pntr->println("<!DOCTYPE HTML>");
  client_pntr->println("<html>");
  client_pntr->println("<body>");
  client_pntr->print("<h1>Hello, World! current time is : ");client_pntr->print(millis());client_pntr->println("</h1>");
  client_pntr->println("</body>");
  client_pntr->println("</html>");*/
  //Close the connection
  delay(10);
  //client_pntr->flush();
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

void answerHttp(EthernetClient* client_pntr,String currentLine){
  client_pntr->println("HTTP/1.1 200 OK");
  client_pntr->println("Content-Type: text/html");
  client_pntr->println("Connection: close");  // the connection will be closed after completion of the response
  //client_pntr->println("Refresh: 5");  // refresh the page automatically every 5 sec
  client_pntr->println();
  client_pntr->print("Received command : " + currentLine + " at internal time :");client_pntr->println(millis());
  client_pntr->println();
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
  client_pntr->println("<p><a href=\"http://192.168.0.101/reset\">Reset</a></p>");
  client_pntr->println("</body>");
  client_pntr->println("</html>");
}
