/*
  Firmware for the motor driver console of ROBOT arms
  ==================================================

  Firmware Version: 1.0.0.0
  
  Programmed By   : Mr. Yasiru Senerath Karunanayaka
                    (Engineering (Bsc. Hons) undergraduate university of Moratuwa)

  Date            : 26-Aug-2021


*/

// Include necessary liblaries
#include "MotorDriver.h"
#include "Stepper.h"
#include <SPI.h>
#include <Arduino.h>

// Define pins using in the main program
#define latch 2 // 2 nd in PORTD registwe
#define latchState 3 // Motor state latch

MotorDriver::MotorDriver(StepperMotor *motorRef_){
  /*This is the constructor for the motor driver class*/
  motorRef = motorRef_; // motorref is a pointer to Stepper motor array
  initDriver();
}// end of the motor Driver Constructor

void MotorDriver::initDriver(){
  /*
   DOCSTRING: This function is used to initialize the driver when starts
   ALGORITHM: --> Firstly setup the data direction of the latch pin of shift REG to output
              --> Setup SPI communication to communicate with the driver
              --> Initially calculate the flip periods for given motor references
              --> Extract Directions and build the direction byte

   RETURN   : NULL
  */
  
  // set data direction of latch to output
  DDRD = DDRD | ( 1 << latch);
  DDRD = DDRD | ( 1 << latchState);

  // set SPI communication
  // Setup the SPI communication to communicate with the driver board
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);

  // calculate the flip periods of the data
  claculateFlipPeriods();

  // update the direction byte
  updateDirections();

  // Now cleanup motor history registry
  motorState = 0;
  // now its time to transfer calculated data
  PORTD &= ~(1 << latchState);
  delayMicroseconds(1); // need a delay
  SPI.transfer(motorState); 
  delayMicroseconds(1); // need a delay
  PORTD |= 1 << latchState;
}// end of the initialization of the driver

void MotorDriver::claculateFlipPeriods(){
  /*
   DOCSTRING: This function is used to calculate the flip periods of each motor driving pulses
   ALGORITHM: --> A flip period directly propotinal to the frame speed
              --> For a step minimumly we need two frames. (0 -> 1 -> 0)
              --> Using an equation below we calculate the periods and update each motor reference by ID

   RETURN   : NULL
  */
  for(uint8_t i = 0; i < 8; ++i){
    flipPeriods[motorRef[i].id] = round(float(frameSpeed) / (400 * (motorRef[i].speedRPM / 60.0)));
  }
}// end of the flip period calculation

void MotorDriver::listMotors(){
    /*
   DOCSTRING: This function is used to printout current motor status list when needed
   RETURN   : NULL
  */
  Serial.println(" Listing Down Connected Motors for the Driver");
  Serial.println("==============================================");
  for(uint8_t i = 0; i < 8; ++i) motorRef[i].printMotorData();
  Serial.println("-----------------END LISTING------------------\n");
}// end of listing Motors

void MotorDriver::updateDirections(){
    /*
   DOCSTRING: This function is used to build the direction byte
   ALGORITHM: --> The direction of each motor is represented as 0 or 1
              --> The direction byte id formed by combining all 8 directions together
              --> Since we use MSBFIRST in SPI communication make sure to reverse the byte

   RETURN   : NULL
  */
  directionBits &= 0; //clearing the byte
  for(uint8_t i = 0; i < 8; ++i){
    directionBits = directionBits | motorRef[i].direction_ << i; // shifting and updating bits
  }
}// end of updating motor direction byte

void MotorDriver::setMotorSteps(uint8_t id, short steps, short speedRPM){
    /*
   DOCSTRING: This function is used to set motor steps speed and direction at the run time.
              specifiy the ID of the motor to be manipulated and +/- Steps and Rotation speed > 0
              
   ALGORITHM: --> First set the direction of the motor by checking the polarity of the step variable
              --> Then Set the RPM of the motor
              --> Finally update the GLOBAL step list variable using absolute value of steps
              --> (IMPORTANT) For a single step driver must provide two transitions (1->0, 0->1).
                  since, we have to consider the flipping period

   RETURN   : NULL
  */
  
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
  motorState |= 1 << id; // update motor state register

  // now its time to transfer calculated data
  PORTD &= ~(1 << latchState);
  delayMicroseconds(1); // need a delay
  SPI.transfer(motorState); 
  delayMicroseconds(1); // need a delay
  PORTD |= 1 << latchState;
  
}// end of setting motor steps

unsigned int* MotorDriver::findMaxSteps(){
   /*
   DOCSTRING: This function is used to find the number of iterations needed to spin motors in
              the motor spin function
              
   ALGORITHM: --> Use and maximum finding fast algorithm to fin the maximum out of steps list

   RETURN   : returns the pointer to the calculated maximum number of steps variable
  */
    maxSteps = 0;
    for(uint8_t i = 0; i < 8; ++i) {
      if (maxSteps < steps_list[i]) maxSteps = steps_list[i];
    }
    return &maxSteps; 
}

void MotorDriver::execute(){
  /*
   DOCSTRING: This function is used to execute the settled steps sirections and speed for the 
              motors.
              
   ALGORITHM: --> Use step finding function to find number of itterations needed
              --> Do the folowing process for the number of itterations

              --> Get the direction byte and distribute the byte among 2 bytes 1 by 1 in equal distance
                  (11001010) -> _1_1_0_0_1_0_1_0

              --> Then fill in remaining bits according to flipping periods (flip each bit when flip period arrvies)
                  (When doing that make sure you don't flip if allocated steps for a motor is ended)

              --> After each itteration reduce the number of steps remain in the steps list by one

              --> Send the composit data over SPI to the driver

              --> Calculate the time taken and delay remaining time until frame period completes

   RETURN   : returns the pointer to the calculated maximum number of steps variable
  */
  
  unsigned int counter = *findMaxSteps(); // counter will count the number of frames occured
  unsigned short composit = 0; // this is the final 2 bit data to be sent to shoft registers
  long time_ {}; // this variable is used to calculate the consumptioned time for the process in micro seconds
  
  while (counter){
    time_ = micros();
    for(uint8_t i = 0; i < 8; ++i){
      composit |= (1 & directionBits >> i) << (2 * i); // assign direction bits
      composit ^= (!(counter % flipPeriods[i]) & !!steps_list[i]) << (2 * i + 1); // assign pulses
      if (steps_list[i]) steps_list[i] --; //decrement the steps 
    } // end of the 8 for loop

    // now its time to transfer calculated data
    PORTD &= ~(1 << latch);
    delayMicroseconds(1); // need a delay
    SPI.transfer16(composit); 
    delayMicroseconds(1); // need a delay
    PORTD |= 1 << latch;

    counter --; // deccrement the counter
    
    time_ = micros() - time_; // calculate the time taken for the process
    delayMicroseconds(framePeriod - time_); // delay to fill the frame period
  }// end of the main while loop

  // Now cleanup motor history registry
  motorState = 0;
  // now its time to transfer calculated data
  PORTD &= ~(1 << latchState);
  delayMicroseconds(1); // need a delay
  SPI.transfer(motorState); 
  delayMicroseconds(1); // need a delay
  PORTD |= 1 << latchState;
}// end of motor spinning function
