# !usr/bin/python

# Include necessary liblaries
import RPi.GPIO as GPIO
from datetime import datetime
from threading import Thread
from time import sleep
import numpy as np

def threaded(fn):
    def wrapper(*args, **kwargs):
        thread = Thread(target=fn, args=args, kwargs=kwargs)
        thread.start()
        return thread
    return wrapper

class Stepper:
    def __init__(self, id_, speedRPM = 60, direction = False):
        # Maximum allowed number of motors are 8 therefor id should between 0->7(inclusive)
        self.id = id_
        self.dir_pin = id_ # the direction pin
        self.step_pin = id_ + 1 # stepping pin
        
        self.speedRPM = speedRPM
        self.duration = 1/((self.speedRPM / 60) * 200) * 1e6  # period of a pulse in microseconds
        self.speedRPS = self.speedRPM / 60
        self.direction = direction
    
    @threaded
    def updateMotor(self):
        while True:
            direction = bool(input("Enter Direction -> "))
            self.direction = direction

class StepperDriver:
    def __init__(self, motorRef):
        self.conn = {
            'clk': 11, # Serial clock pin
            'ser': 10, # Serial data pin
            'ltc': 22  # Latch pin
        }
        
        self.motorRef = motorRef # a list of motor objects
        self.pulseSpeed = 2670 #minimum requirenment
        self.pulsePeriod = 1/self.pulseSpeed * 1e6
        self.flipPeriods = self.__calculateBitPeriod()
        self.__exit = False

        self.__setupGPIO()

    def __setupGPIO(self):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.conn['clk'], GPIO.OUT)
        GPIO.setup(self.conn['ser'], GPIO.OUT)
        GPIO.setup(self.conn['ltc'], GPIO.OUT)

        GPIO.output(self.conn['ltc'], GPIO.LOW)
        GPIO.output(self.conn['clk'], GPIO.HIGH)

    def __shiftOut(self, val):
        for i in range(0,16):
            GPIO.output(self.conn['clk'], GPIO.HIGH)
            GPIO.output(self.conn['ser'], val[i] and GPIO.HIGH or GPIO.LOW)
            GPIO.output(self.conn['clk'], GPIO.LOW)
        
    def __calculateBitPeriod(self):
        """
        DOCTRING: This function assumes the paralel data output at pulseSpeed and calculate the lipping points
                  each motor
        """
        flips = [0,0,0,0,0,0,0,0]
        
        for motor in self.motorRef:
            flips[motor.id] = round(self.pulseSpeed / (400 * motor.speedRPS))
            
        return flips
            

    @threaded
    def stepMotors(self):
        """
        DOCSTRING: Since we must approch 6.667 RPS from a motor, Pulsing Rate -> 6.667 * 200 -> 1333.4 Hz but pulse 
                   includes 1,0 Combination therefor step data must be generated in 2670 Hz minimum.
        """
        
        # FORMAT-> M0 M1 M2 M3 M4 M5 ..
        #       -> DS Ds Ds Ds Ds Ds ..
        
        __counter = 0 # The Flip Counter
        data = np.zeros(shape = (16), dtype=bool)
        
        while not self.__exit:
            start = datetime.now()

            for i in range(len(self.motorRef)):
                data[2 * i] = self.motorRef[i].direction
                data[2 * i + 1] ^= (__counter % self.flipPeriods[i] == 0)

            GPIO.output(self.conn['ltc'], GPIO.HIGH)
            self.__shiftOut(data[::-1])
            GPIO.output(self.conn['ltc'], GPIO.LOW)

            try:
                sleep((self.pulsePeriod - (datetime.now() - start).microseconds - 100) / 1e6)
            except ValueError:
                pass

            __counter += 1 # increment the coounter
            # print(data)
            # sleep(30)

        GPIO.cleanup()


    @threaded
    def driverConsole(self):
        while not self.__exit:
            data = input("Driver Console#/ > ")
            if data == 'quit()':
                self.__exit = True

    def start(self):
        self.stepMotors()
        self.driverConsole()



driver = StepperDriver([Stepper(0), Stepper(1, speedRPM=120, direction=True), Stepper(2, speedRPM=240)])
driver.start()

    