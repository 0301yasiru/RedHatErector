#ifndef StepperFunctions
#define StepperFunctions

#include <stdint.h>

class StepperMotor{
  private:
    // There will be no private variables or methods for this class
  
  public:
    //Defining the constructor for the class
    StepperMotor(uint8_t id_, short speedRPM_ = 60, bool direction__ = false);
    void printMotorData();

    uint8_t id {};
    short speedRPM {};
    bool direction_ {};
//  
}; //End of the Stepper Motor class

#endif
