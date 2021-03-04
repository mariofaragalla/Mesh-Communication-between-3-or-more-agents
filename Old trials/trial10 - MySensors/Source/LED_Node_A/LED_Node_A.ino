/********************************************************
 * Simple Node with an LED
 * Ralf Stoffels 1/2016
 *   
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 ********************************************************/
#include <MySensor.h>
#include <SPI.h>

// LED A
#define NODE_ID 11   //This is the individual address of this node

#define LED  A0  // Arduino Digital I/O pin number for LED
#define CHILD_ID_LED 1

// Construct MySensors library
MySensor gw;

void setup()  
{   
  // Initialize library and add callback for incoming messages
  gw.begin(incomingMessage, NODE_ID, true, AUTO);
  gw.sendSketchInfo("LED A", "1.0");
  gw.present(CHILD_ID_LED, S_LIGHT);

  pinMode(LED, OUTPUT);   
  digitalWrite(LED,LOW);

}


void loop() 
{
  // Alway process incoming messages whenever possible
  gw.process();
}

void incomingMessage(const MyMessage &message) {
     if (message.getBool() == true) digitalWrite(LED, HIGH);
     else digitalWrite(LED, LOW);
}

