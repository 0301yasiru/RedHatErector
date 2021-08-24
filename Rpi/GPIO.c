#include <wiringPi.h>

#define latch_ 22
#define serial_ 10
#define clock_ 11

void setup(){
    wiringPiSetupGpio();
        
    // pinMode(latch_, OUTPUT);
    pinMode(serial_, OUTPUT);
    pinMode(clock_, OUTPUT);
}


void shiftData(short data){
	// digitalWrite(latch_, 1);
	for (short i = 0; i < 8; ++i){
		digitalWrite(clock_, 1);
		delayMicroseconds(1);
		digitalWrite(serial_, 1 & data >> i);
		delayMicroseconds(1);
        digitalWrite(clock_, 0);
		delayMicroseconds(1);
	} 
	// digitalWrite(latch_, 0);
}//end of the function



