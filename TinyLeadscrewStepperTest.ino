// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define a stepper and the pins it will use
AccelStepper stepper; // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

int max_max_speed = 2000;   // max AccelStepper can do on a Nano, according to my experiments - actually end up with 1470 steps/sec
int max_acceleration = 20000; // really a lot - set this higher than about 20000 and accelstepper doesn't seem to run at all
int max_pos = 550;

#define STATE_HOMING 0
#define STATE_BLINK_OPEN 1
#define STATE_BLINK_CLOSE 2
#define STATE_SLOW_OPEN 3
#define STATE_SLOW_CLOSE 4
int state;

void home()
{
  state = STATE_HOMING;
  
  stepper.setMaxSpeed(max_max_speed / 10);
  stepper.setAcceleration(100);
  stepper.moveTo( - max_pos);
  
}

void blink_open()
{
  state = STATE_BLINK_OPEN;
  
  stepper.setMaxSpeed(max_max_speed);
  stepper.setAcceleration(max_acceleration);
  stepper.moveTo( max_pos);
  
}

void blink_close()
{
  state = STATE_BLINK_CLOSE;
  
  stepper.setMaxSpeed(max_max_speed);
  stepper.setAcceleration(max_acceleration);
  stepper.moveTo( 0 );
  
}

void slow_open()
{
  state = STATE_SLOW_OPEN;
  
  stepper.setMaxSpeed(max_max_speed/10);
  stepper.setAcceleration(max_acceleration/100);
  stepper.moveTo( max_pos/2);
  
}

void slow_close()
{
  state = STATE_SLOW_CLOSE;
  
  stepper.setMaxSpeed(max_max_speed/10);
  stepper.setAcceleration(max_acceleration/10);
  stepper.moveTo( 0 );
  
}
void setup()
{  
  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(100000);
  
  home();

  Serial.begin(115200);
}

long lastMillis = 0;
long calls = 0;

int last_pos = 0;



void loop()
{
    // If at the end of travel go to the other end
    if (stepper.distanceToGo() == 0)
    {
      long now = millis();

      long steps = abs(last_pos - stepper.currentPosition());
      Serial.print("Steps : ");
      Serial.println(steps);
      
      long stepsPerSec = (steps * 1000) / (now-lastMillis);
      Serial.print("Steps per sec: ");
      Serial.println(stepsPerSec);

      long callsPerSec = (calls * 1000) / (now-lastMillis);
      Serial.print("Calls per sec: ");
      Serial.println(callsPerSec);

      calls = 0;
      last_pos = stepper.currentPosition();
      lastMillis = now;

      switch( state )
      {
        case STATE_HOMING:
          stepper.setCurrentPosition(0);
          blink_open();
          break;

        case STATE_BLINK_OPEN:
          blink_close();
          break;

        case STATE_BLINK_CLOSE:
          slow_open();
          break;    
          
        case STATE_SLOW_OPEN:
          slow_close();
          break;

        case STATE_SLOW_CLOSE:
          blink_open();
          break;    
      }

      Serial.print("State: ");
      Serial.println(state);
    }

   
    stepper.run();

  /*
    // commenting these in will ruin performance:
    Serial.print("stepper.currentPosition(): ");
    Serial.println(stepper.currentPosition());
    Serial.print("stepper.distanceToGo() : ");
    Serial.println(stepper.distanceToGo() );
*/
      
    calls ++;
}
