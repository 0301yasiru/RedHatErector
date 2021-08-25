#ifndef MotorDriverFunctions
#define MotorDriverFunctions

#include "Stepper.h"

class MotorDriver{
  private:
    // these are the private variables will be used inside the driver
    StepperMotor *motorRef {};
    int frameSpeed = 2670; // this is the frequancy of updating the bit stream in shift regiseters
    short framePeriod = 1e6 / frameSpeed;  // this is the period of a frame

    uint8_t flipPeriods[8] {}; // this array will stor the period of each pulse bit
    unsigned int steps_list[8]; // this function will store the number of steps assaigned for each motor
    uint8_t directionBits {};  // this byte will contain the directions of each motor
    unsigned int maxSteps = 0; // this variable will be use to calculate step sum
  
    // init Driver function will be used to initialize the driver
    void initDriver();
    void claculateFlipPeriods();
    void updateDirections();
    unsigned int* sumSteps();
    

  public:  
    // definition of the motor driver constructor
    MotorDriver(StepperMotor *motorRef_);

    
    // public functions are defined here
    void listMotors();
    void setMotorSteps(uint8_t id, short steps = 0, short speedRPM = 0);
    void spinMotors();
    
}; // end of the motor Driver class

#endif
