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
	//Steinhart Hart Equation:
	// T = 1/(a + b[ln(ohm)] + c[ln(ohm)]^3)

	double volts = (rawData / 1023)*3.3;
	double ohms = ((3.3*10000)-(volts*10000))/volts;
	double lnohm = log(ohms); // = ln(x)
	
	// R@25°C=8600, R (-6.2%/C @ 25C) Mil Ratio X
	double a = 0.001860902033483;
	double b = 0.000156942702230;
	double c = 0.000000094148064;
	
	double t1 = (b*lnohm); // b[ln(ohm)]
	double c2 = c*lnohm; // c[ln(ohm)]
	double t2 = pow(c2,3); // c[ln(ohm)]^3
	double temp = 1/(a + t1 + t2); //calcualte temperature
	int tempc = temp - 273.15 - 4; //K to C
	
	//cout << " (U:" << volts << " R:" << ohms << ") ";
	return tempc;
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