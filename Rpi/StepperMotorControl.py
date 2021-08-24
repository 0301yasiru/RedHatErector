# !usr/bin/python

# Include necessary liblaries
import RPi.GPIO as GPIO
from datetime import datetime
from threading import Thread
from time import sleep
import numpy as np
from ctypes import *

cfunc = CDLL('/home/pi/Documents/Programs/Python/RedHatErector/Rpi/GPIO.so')

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
        self.direction = direction
    
    @threaded
    def updateMotor(self):
        while True:
            direction = bool(input("Enter Direction -> "))
            self.direction = direction

class StepperDriver:
    def __init__(self, motorRef):
        """
        DOCSTRING: This function will initialize the Stepper Driver Unit
        """
        self.conn = {
            'clk': 11, # Serial clock pin
            'ser': 10, # Serial data pin
            'ltc': 22  # Latch pin
        }
        
        self.motorRef = motorRef # a list of motor objects

        # now we have to fill in the empty motors
        for tempID in range(8):
            if tempID not in [motor.id for motor in self.motorRef]:
                self.motorRef.append(Stepper(tempID))
                
        
        self.pulseSpeed = 2670 #minimum requirenment
        self.pulsePeriod = 1/self.pulseSpeed * 1e6

        # extract motorDirections
        self.__motorDirections = np.array([motor.direction for motor in motorRef], dtype='int8')
        # calculate bit flip periods
        self.flipPeriods = self.__calculateBitPeriod()
        
        self.__setupGPIO()
        self.__exit = False

    def __setupGPIO(self):
        cfunc.setup()
        GPIO.setmode(GPIO.BCM)
        # GPIO.setup(self.conn['clk'], GPIO.OUT)
        # GPIO.setup(self.conn['ser'], GPIO.OUT)
        GPIO.setup(self.conn['ltc'], GPIO.OUT)

        # GPIO.output(self.conn['ltc'], GPIO.LOW)
        # GPIO.output(self.conn['clk'], GPIO.HIGH)

        
    def __calculateBitPeriod(self):
        """
        DOCTRING: This function assumes the paralel data output at pulseSpeed and calculate the lipping points
                  each motor
        """
        flips = np.ones(shape = 8, dtype = 'int8')
        
        for motor in self.motorRef:
            flips[motor.id] = round(self.pulseSpeed / (400 * motor.speedRPM / 60))
            
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
            time = datetime.now()
            
            data[::2] = self.__motorDirections
            data[1::2] ^= __counter % self.flipPeriods == 0
            
            data_packed = np.packbits(data)
            
            GPIO.output(self.conn['ltc'], GPIO.HIGH)

            cfunc.shiftData(int(data_packed[1]))
            cfunc.shiftData(int(data_packed[0]))

            GPIO.output(self.conn['ltc'], GPIO.LOW)

            __counter += 1 # increment the coounter
            time = datetime.now() - time
            
            try:
                sleep((self.pulsePeriod - time.microseconds - 50) * 1e-6)
            except ValueError:
                pass


        GPIO.cleanup()

    def __printMotors(self):
        print("+-------------+-------------+-------------+")
        print("|  MOTOR ID   |  SPEED RPM  |  DIRECTION  |")
        print("+-------------+-------------+-------------+")
        
        for motor in self.motorRef:
            print("|" + str(motor.id).ljust(13) + "|" + str(motor.speedRPM).ljust(13) + "|" + str(motor.direction).ljust(13) + "|")
        
        print("+-------------+-------------+-------------+")

    def __setAttr(self, id_, speed=None):
        if speed:
            self.motorRef[id_].speedRPM = speed
            self.flipPeriods = self.__calculateBitPeriod()
            

    @threaded
    def driverConsole(self):
        while not self.__exit:
            data = input("Driver Console#/ > ").split()
            
            if data[0] == 'quit()':
                self.__exit = True

            elif data[0] == 'list' or data[0] == 'show':
                if data[1] == 'motors':
                    self.__printMotors()

            elif data[0] == 'set':
                motorid = int(data[1][-1])
                
                if data[2] == 'speed':
                    self.__setAttr(motorid, speed = int(data[3]))
                
                elif data[2] == 'dir':
                    self.motorRef[motorid].direction = bool(int(data[3]))
                    self.__motorDirections = np.array([motor.direction for motor in self.motorRef], dtype='int8')

            

    def start(self):
        self.stepMotors()
        self.driverConsole()



driver = StepperDriver([Stepper(0), Stepper(1), Stepper(2)])
driver.start()

    