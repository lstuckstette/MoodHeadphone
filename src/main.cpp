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
#include "Button.cpp"

#define PULSESENSOR_CHANNEL 1
#define LIGHTSENSOR_CHANNEL 0
#define DHT_GPIO 29 //BCM_GPIO21 -->see gpio readall

#define BUTTON_1_GPIO 28 // wiring pi number!!! (upper button)
#define BUTTON_2_GPIO 25
#define BUTTON_3_GPIO 23
#define BUTTON_4_GPIO 26
#define BUTTON_5_GPIO 22

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

void buttonTest(int pin){
	cout << "Button " << pin << "pressed!" << endl;
}

int main(void) {
	
	initialize();
	
	Button b;
	b.registerCallback(BUTTON_1_GPIO, &buttonTest);
	b.registerCallback(BUTTON_2_GPIO, &buttonTest);
	b.registerCallback(BUTTON_3_GPIO, &buttonTest);
	b.registerCallback(BUTTON_4_GPIO, &buttonTest);
	b.registerCallback(BUTTON_5_GPIO, &buttonTest);
	
    Pulse p(PULSESENSOR_CHANNEL);
    p.startReading();
	Light l(LIGHTSENSOR_CHANNEL);
	l.startReading();
	DHT22 t(DHT_GPIO);
	t.startReading();
    while (true) {        
        //cout << "reading " << p.getBPM() << " BPM!" << endl;
		cout << "DHT22: " << t.getTemperature() << "C" << " and Humidity: " << t.getHumidity() << "%."<< endl;
		cout << "Lightsensor: " << l.getBrightness() << " (raw)" << endl;
		cout << "Pulsesensor: " << p.getBPM() << " (bpm)" << endl;
		cout << "------------------------------------------" << endl;
        this_thread::sleep_for(milliseconds(1000));
    }
    //p.stopReading();
}


