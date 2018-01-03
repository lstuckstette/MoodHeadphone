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
#define LIGHT_SLEEP_DELAY 50
#define MAX_ADC_READING 1023
#define ADC_REF_VOLTAGE 3.3
#define REF_RESISTANCE 10000 
#define LUX_CALC_SCALAR 65721  //seeded
#define LUX_CALC_EXPONENT -0.7081 //seeded

/*
    Author: Lukas Stuckstette
*/

using namespace std;
using namespace std::chrono;

class Light {
    public:
        Light(int);
		~Light(void);
		void startReading(void);
		void stopReading(void);
		int getBrightness(void);
		int getLux(void);

    private:
		thread loop;
		int channel;
		double rawData=0;
		void lightLoop(void);

};

void Light::lightLoop(void){
	while(true){
		rawData=analogRead(ADC_BASE + channel);
		this_thread::sleep_for(milliseconds(LIGHT_SLEEP_DELAY));	
	}
}

int Light::getLux(){
	cout << "raw: " << rawData;
	
	double resistorVoltage = (double)rawData/ MAX_ADC_READING * ADC_REF_VOLTAGE;
	
	double ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
	
	double ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
	
	cout << " ldrResistance: " <<  ldrResistance << "  -  ";
	
	double ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
	
	return (int)ldrLux;
	
}
int Light::getBrightness(void){
	return (int)rawData;
}

Light::Light(int adc_channel) {
    //constructor
    channel = adc_channel;
}

Light::~Light(void) {
    //clean memeory
    stopReading();
}

void Light::startReading() {
    //init, start loop reading values;
	
    loop = thread( & Light::lightLoop, this);
}

void Light::stopReading(void) {
    loop.detach();

}