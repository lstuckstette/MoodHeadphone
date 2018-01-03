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
#define LIGHT_SLEEP_DELAY 10
#define MAX_ADC_READING 1023
#define ADC_REF_VOLTAGE 3.3
#define REF_RESISTANCE 10 //kohm
#define LUX_CALC_SCALAR 12518931  //seeded
#define LUX_CALC_EXPONENT -1.405 //seeded
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
		double rawData;
		void lightLoop(void);

};

void Light::lightLoop(void){
	int tmp=0;
	while(true){
		for(int i=0;i<10;i++){
			tmp+=analogRead(ADC_BASE + channel);
			this_thread::sleep_for(milliseconds(LIGHT_SLEEP_DELAY));
		}
		rawData = tmp/10;
		tmp=0;
		
	}
}

int Light::getLux(){
	
	/*float resistorVoltage = (float)rawData / MAX_ADC_READING * ADC_REF_VOLTAGE;
	float ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
	float ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
	cout << "LDR_RES " << ldrResistance << endl;
	float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
	return (int) ldrLux;*/
	return 0;
	
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