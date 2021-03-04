/********************************************************
 * Most simplified version of a Gateway with serial 
 * interface to a PC
 * Ralf Stoffels 1/2016
 * 
 * 'A' via serial switches the LED of the LED_Node_A on 
 * 'a' via serial switches the LED of the LED_Node_A on 
 * 'B' via serial switches the LED of the LED_Node_B on 
 * 'b' via serial switches the LED of the LED_Node_B on 
 * All incomming messages are printed over serial
 *   
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 ********************************************************/
 
#include <SPI.h>
#include <MySensor.h>  

#define REPEATER_NODE 10   // pre defined addresses of my little test network
#define LED_NODE_A 11
#define LED_NODE_B 13
#define SENSOR_NODE 12

MySensor gw;
MyMessage msg(0, S_LIGHT);

void setup()  
{ 
  gw.begin(incomingMessage,0, true, 0);
  Serial.begin(115200);
}

void loop()      
{ 
  bool value;
  int c;
  int node = 0;
  
  gw.process();

  if (Serial.available() > 0) {
    c = Serial.read();
    if ((c == 'A')||(c == 'B')) value = true; else value = false;
    if ((c == 'A')||(c == 'a')) node=LED_NODE_A;
    if ((c == 'B')||(c == 'b')) node=LED_NODE_B;
    
    Serial.print("-->"); Serial.println(value);
    msg.setType(S_LIGHT);
    msg.setSensor(1);
    msg.setDestination(node);
    msg.set(value);
    gw.send(msg);
  }
}

void incomingMessage(const MyMessage &msg) {
  Serial.println("Message received...");
}


