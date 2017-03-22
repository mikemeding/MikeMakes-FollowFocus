// Speed Adjusting Stepper controller for use with camera zoom/focus
// tested and working on an Arduino MEGA 2560
// Author: Mike Meding (www.mikemeding.com)
// Copyright (C) 2016 Mike Meding

// PARTS:
// - Allegro stepper driver
// - 10k pot
// - end stop microswitches (pull-up)
// - NEMA 11 or 17 stepper motor

#include <AccelStepper.h>

// pin definitions
#define STEP (5)
#define DIR (4)
#define NEGEND (2) // INT0
#define POSEND (3) // INT1
#define POT (A1)

float potValue = 0;
byte nopTick = 0;
AccelStepper stepper(1, STEP, DIR); // 1 = (2 wire mode)

void setup()
{
  // setup endstop buttons as interrupts
  pinMode(NEGEND, INPUT_PULLUP);
  pinMode(POSEND, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(NEGEND), negEndStop, CHANGE);
  attachInterrupt(digitalPinToInterrupt(POSEND), posEndStop, CHANGE);

  // setup motor
  stepper.setMaxSpeed((float)(1023*2));
  
  // setup analog pot
  pinMode(POT, INPUT);

  // =====================================================
  // SERIAL FUCKS UP PWM COUNTER FOR MOTOR!!!!!
  // DO NOT USE UNLESS TO DEBUG!!!!!!
  // =====================================================
  //Serial.begin(9600);

}

void loop()
{

  stepper.runSpeed(); // call as often as possible
  stepper.setSpeed((float)readAndConvertPot());

}

/**
 * Reads from the analog potentiometer a value 0-5v (0-1023)
 * then converts it to center 0 for a positive negative sweeping motion.
 * Output value becomes (-1023 +1023)
 */
int readAndConvertPot() {
  int delta = 120;
  int value = (analogRead(POT) * 8) - (1023*4);
  if (value < delta && value > -delta) {
    return 0;
  } else {
    return value > 0? value -delta: value+delta;
  }
}

/**
 * Interrupt for stopping the negative rotation. Holds the motor captive until
 * readAndConvertPot() returns a positive value (IE: spinning the other direction)
 */
void negEndStop() {
  stepper.stop();
  while (readAndConvertPot() > 0) { // "captive" wait for value to come back down (reverse direction)
    nopTick++; // super short delay tick (1-2 cycles)
  }
}
void posEndStop() {
  stepper.stop();
  while (readAndConvertPot() < 0) { // "captive" wait for value to come back down (reverse direction)
    nopTick++; // super short delay tick (1-2 cycles)
  }
}


