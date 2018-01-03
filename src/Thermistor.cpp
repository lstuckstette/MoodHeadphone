#include <iostream> 
#include <string> 
#include <thread> 
#include <ctime> 
#include <chrono>
#include <cmath>
//includes for SPI/ADC
#include <wiringPi.h> 
#include <mcp3004.h> // works for mpc3008 as well

#define ADC_BASE 100 //Base for ADC read
#define TMP_SLEEP_DELAY 50

/*
    Author: Lukas Stuckstette
*/

using namespace std;
using namespace std::chrono;

class Temp{
	public:
	    Temp(int);
		~Temp(void);
		void startReading(void);
		void stopReading(void);
		int getRaw(void);
		int getCelsius(void);

    private:
		thread loop;
		int channel;
		double rawData;
		void tempLoop(void);
};

void Temp::tempLoop(void){

	while(true){
		rawData=analogRead(ADC_BASE + channel);
		this_thread::sleep_for(milliseconds(TMP_SLEEP_DELAY));
	}
}

int Temp::getRaw(){
	return rawData;
}

int Temp::getCelsius(){
	return rawData;
}

Temp::Temp(int adc_channel) {
    //constructor
    channel = adc_channel;
}

Temp::~Temp(void) {
    //clean memeory
    stopReading();
}

void Temp::startReading() {
    //init, start loop reading values;
	
    loop = thread( & Temp::tempLoop, this);
}

void Temp::stopReading(void) {
    loop.detach();

}