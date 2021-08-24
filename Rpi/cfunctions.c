#include <wiringPi.h>
#include <stdio.h>

#define latch_ 22
#define serial_ 10
#define clock_ 11

void shiftData(_Bool *data){
	// digitalWrite(latch_, 1);
	for (short i = 0; i < 16; ++i){
		digitalWrite(clock_, 1);
		delayMicroseconds(1);
		digitalWrite(serial_, !data[i]);
		delayMicroseconds(1);
		digitalWrite(clock_, 0);
		delayMicroseconds(1);
	} 
	// digitalWrite(latch_, 0);
}//end of the function

int main(void){
	wiringPiSetupGpio();
	
	pinMode(latch_, OUTPUT);
	pinMode(serial_, OUTPUT);
	pinMode(clock_, OUTPUT);
	
	_Bool data[16] = {1,0,1,0,1,0,1,0,    1,0,1,0,0,0,1,0};
	
	
	unsigned long time2 = 0;
	
	unsigned long time1 = micros();
	shiftData(data);
	time2 = micros() - time1;
	
	
	printf("Time took -> %lu \n", time2);

	
	return 0;
}//end of the main function


