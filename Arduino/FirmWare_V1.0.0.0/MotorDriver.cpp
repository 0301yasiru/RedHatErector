#include "MotorDriver.h"
#include "Stepper.h"
#include <SPI.h>
#include <Arduino.h>

// Define pins using in the main program
#define latch 2 // 2 nd in PORTD registwe

MotorDriver::MotorDriver(StepperMotor *motorRef_){
  motorRef = motorRef_; // motorref is a pointer to Stepper motor array
  initDriver();
}// end of the motor Driver Constructor

void MotorDriver::initDriver(){
  // set data direction of latch to output
  DDRD = DDRD | ( 1 << latch);

  // set SPI communication
  // Setup the SPI communication to communicate with the driver board
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);

  // calculate the flip periods of the data
  claculateFlipPeriods();

  // update the direction byte
  updateDirections();
}// end of the initialization of the driver

void MotorDriver::claculateFlipPeriods(){
  for(uint8_t i = 0; i < 8; ++i){
    flipPeriods[motorRef[i].id] = round(float(frameSpeed) / (400 * (motorRef[i].speedRPM / 60.0)));
  }
}// end of the flip period calculation

void MotorDriver::listMotors(){
  Serial.println(" Listing Down Connected Motors for the Driver");
  Serial.println("==============================================");
  for(uint8_t i = 0; i < 8; ++i) motorRef[i].printMotorData();
  Serial.println("-----------------END LISTING------------------\n");
}// end of listing Motors

void MotorDriver::updateDirections(){
  directionBits &= 0; //clearing the byte
  for(uint8_t i = 0; i < 8; ++i){
    directionBits = directionBits | motorRef[i].direction_ << i; // shifting and updating bits
  }
}// end of updating motor direction byte

void MotorDriver::setMotorSteps(uint8_t id, short steps, short speedRPM){

  // first of all get the polarity of steps and set the direction if not null
  if (steps){
    if (steps > 0) motorRef[id].direction_ = false;
    else motorRef[id].direction_ = true;
    updateDirections();
  }
  
  // then set motor speed if not null
  if (speedRPM){
    motorRef[id].speedRPM = speedRPM;
    claculateFlipPeriods();
  }
  
  steps_list[id] = abs(steps) * flipPeriods[id] * 2; // update the step list array 
}// end of setting motor steps

unsigned int* MotorDriver::sumSteps(){
    maxSteps = 0;
    for(uint8_t i = 0; i < 8; ++i) {
      if (maxSteps < steps_list[i]) maxSteps = steps_list[i];
    }
    return &maxSteps; 
}

void MotorDriver::spinMotors(){
  unsigned int counter = *sumSteps(); // counter will count the number of frames occured
  unsigned short composit = 0; // this is the final 2 bit data to be sent to shoft registers
  long time_ {};
  while (counter){
    time_ = micros();
    for(uint8_t i = 0; i < 8; ++i){
      composit |= (1 & directionBits >> i) << (2 * i); // assign direction bits
      composit ^= (!(counter % flipPeriods[i]) & !!steps_list[i]) << (2 * i + 1); // assign pulses
      if (steps_list[i]) steps_list[i] --; //decrement the steps 
    } // end of the 8 for loop

    // now its time to transfer calculated data
//    PORTD |= 1 << latch; // set latch pin on arduino to HIGH (this will invert at shift register)
    PORTD &= ~(1 << latch);
    delayMicroseconds(1); // need a delay
    SPI.transfer16(composit); 
    delayMicroseconds(1); // need a delay
    PORTD |= 1 << latch;
//    PORTD &= ~(1 << latch); // set latch pin on arduino to LOW (this will invert at shift register)

    
    counter --; // deccrement the counter
    
    time_ = micros() - time_; // calculate the time taken for the process
    delayMicroseconds(framePeriod - time_); // delay to fill the frame period
  }// end of the main while loop
}// end of motor spinning function
