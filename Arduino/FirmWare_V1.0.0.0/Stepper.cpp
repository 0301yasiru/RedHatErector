/*
  Firmware for the motor driver console of ROBOT arms
  ==================================================

  Firmware Version: 1.0.0.0
  
  Programmed By   : Mr. Yasiru Senerath Karunanayaka
                    (Engineering (Bsc. Hons) undergraduate university of Moratuwa)

  Date            : 26-Aug-2021


*/

#include "Stepper.h"
#include <Arduino.h>

StepperMotor::StepperMotor(uint8_t id_, short speedRPM_, bool direction__){
    id = id_;
    speedRPM = speedRPM_;
    direction_ = direction__;
}// end of the Steppermotor initialization

void StepperMotor::printMotorData(){
  Serial.println("MotorID -> " + String(id) + "\tSpeedRPM -> " + String(speedRPM) + "\tDirection -> " + String(direction_));
}// end of printing stepper motor data
