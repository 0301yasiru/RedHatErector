
// Include necessary liblaries for the project
#include "MotorDriver.h"
#include "Stepper.h"


// Create instances of classes

// We are to use 4 stepper motors for each arm. There for we need 8 motors
StepperMotor motorRef[8] {StepperMotor(0, 200), StepperMotor(1, 200), StepperMotor(2, 80, 1), StepperMotor(3),
                          StepperMotor(4), StepperMotor(5, 250, 1), StepperMotor(6, 60, 1), StepperMotor(7, 60, 1)};

// Creating the instance for motor driver
MotorDriver driver(motorRef);

void setup() {
  // Setup Serial Communication to communicate with the computer
  Serial.begin(9600);
  
  driver.listMotors();


  Serial.println("Done!");
}

void loop() {
  
  driver.setMotorSteps(1, 10000, 350);
  driver.spinMotors();

  delay(1000);

}
