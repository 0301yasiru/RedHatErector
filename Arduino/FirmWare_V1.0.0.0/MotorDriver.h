/*
  Firmware for the motor driver console of ROBOT arms
  ==================================================

  Firmware Version: 1.0.0.0
  
  Programmed By   : Mr. Yasiru Senerath Karunanayaka
                    (Engineering (Bsc. Hons) undergraduate university of Moratuwa)

  Date            : 26-Aug-2021


*/

#ifndef MotorDriverFunctions
#define MotorDriverFunctions

#include "Stepper.h"

class MotorDriver{
  private:
    // these are the private variables will be used inside the driver
    StepperMotor *motorRef {};
    int frameSpeed = 2670; // this is the frequancy of updating the bit stream in shift regiseters
    short framePeriod = 1e6 / frameSpeed;  // this is the period of a frame

    uint8_t motorState = 0; // this number will hold which motors should be turned on
    uint8_t flipPeriods[8] {}; // this array will stor the period of each pulse bit
    unsigned int steps_list[8]; // this function will store the number of steps assaigned for each motor
    uint8_t directionBits {};  // this byte will contain the directions of each motor
    unsigned int maxSteps = 0; // this variable will be use to calculate step sum
  
    // thse are the private functions to be used inside the class
    void initDriver(); // init Driver function will be used to initialize the driver
    void claculateFlipPeriods(); // this function will calculat flipping periods
    void updateDirections(); // this function will update direction word
    unsigned int* findMaxSteps(); // this function will find the maximum itterations
    

  public:  
    // definition of the motor driver constructor
    MotorDriver(StepperMotor *motorRef_);

    
    // public functions are defined here
    void listMotors(); // this function will list down connected motor status
    void setMotorSteps(uint8_t id, short steps = 0, short speedRPM = 0); // this function will setup a motor
    void execute(); // this function will execute the driver for current settings
    void softExecute();
    
}; // end of the motor Driver class

#endif
