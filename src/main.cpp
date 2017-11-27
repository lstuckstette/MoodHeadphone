//libs
#include <iostream> 
#include <thread> 
#include <chrono>
#include <wiringPi.h> 
#include <mcp3004.h>
//custom
#include "Pulsesensor.cpp"
#include "Lightsensor.cpp"
#include "DHT22.cpp"

#define PULSESENSOR_CHANNEL 1
#define LIGHTSENSOR_CHANNEL 0
#define DHT_GPIO 1

using namespace std;
using namespace std::chrono;


void initialize(void){
	//initialize wiringPi
	if (wiringPiSetup () == -1){
		 cerr << "ERROR ON WIRINGPI_SETUP!!!!" << endl;
		 exit (1) ;
	 }
	 //initialize ADC
	 mcp3004Setup(ADC_BASE, 0);
}

int main(void) {
	
	initialize();
	
    Pulse p(PULSESENSOR_CHANNEL);
    p.startReading();
	
	Light l(LIGHTSENSOR_CHANNEL);
	l.startReading();
    while (true) {
        
        //cout << "reading " << p.getBPM() << " BPM!" << endl;
		cout << "reading " << l.getBrightness() << endl;
        this_thread::sleep_for(milliseconds(500));
    }
    p.stopReading();
}