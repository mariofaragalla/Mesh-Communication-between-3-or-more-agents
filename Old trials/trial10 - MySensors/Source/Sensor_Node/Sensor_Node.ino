/********************************************************
 * Simple version of a Sensor 
 * Here implemented with a switch connected to a 
 * digital pin (just a binary sensor)
 * Ralf Stoffels 1/2016
 *   
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 ********************************************************/
 
#include <SPI.h>
#include <MySensor.h>  

#define NODE_ID 12   //This is the individual address of this node
#define LED_NODE_A 11   // Used to send messages directly to the LED

#define CHILD_ID_SWITCH 0
#define INPUT_PIN A0

unsigned long SLEEP_TIME = 100; // Sleep time between reads (in milliseconds)
bool value = false;  

MySensor gw;
MyMessage msg(CHILD_ID_SWITCH, V_LIGHT_LEVEL);

void setup()  
{ 
  gw.begin(NULL, NODE_ID, true, AUTO);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Simple Switch", "1.0");

  // Register all sensors to gateway (they will be created as child devices)
  gw.present(CHILD_ID_SWITCH, S_LIGHT_LEVEL);

  // Input Pin for Switch
  pinMode(INPUT_PIN,INPUT);
}

void loop()    
{ 

  if (digitalRead(INPUT_PIN) == LOW) {
    msg.setType(S_LIGHT);
    msg.setSensor(1);
    msg.setDestination(LED_NODE_A);   // Send to LED Node
    msg.set(value);
    gw.send(msg);
    
    //if (value == true) value=false; else value=true;
    value = !value;   // toggle

    gw.sleep(SLEEP_TIME);
  }
}



