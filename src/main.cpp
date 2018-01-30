//libs
#include <iostream> 
#include <thread> 
#include <chrono>
#include <wiringPi.h> 
#include <mcp3004.h>
//custom
#include "MediaPlayer.cpp"
#include "Bluetooth.cpp"
#include "Pulsesensor.cpp"
#include "Lightsensor.cpp"
#include "Thermistor.cpp"
#include "Button.cpp"

#define LIGHTSENSOR_CHANNEL 3
#define PULSESENSOR_CHANNEL 1
#define TEMPSENSOR_CHANNEL  2
//#define DHT_GPIO 29 //BCM_GPIO21 -->see gpio readall

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
	cout << "Button " << pin << " pressed!" << endl;
}

int main(void) {
	
	initialize();
	
	
	//MediaPlayer::playSong("guerilla project");
	//MediaPlayer::playPlaylist("Metal");
	/*
	Button b;
	b.registerCallback(BUTTON_1_GPIO, &MediaPlayer::increaseLoudness);
	b.registerCallback(BUTTON_2_GPIO, &MediaPlayer::nextSong);
	b.registerCallback(BUTTON_3_GPIO, &MediaPlayer::tooglePlay);
	b.registerCallback(BUTTON_4_GPIO, &MediaPlayer::previousSong);
	b.registerCallback(BUTTON_5_GPIO, &MediaPlayer::decreaseLoudness);
	*/
	
	
    Pulse p(PULSESENSOR_CHANNEL);
    p.startReading();
	Light l(LIGHTSENSOR_CHANNEL);
	l.startReading();
	Temp t(TEMPSENSOR_CHANNEL);
	t.startReading();
	
	Bluetooth b;
	
	
    while (true) {        
	
		b.setTemp(t.getCelsius());
		b.setLight(l.getLux());
		b.setHeart(p.getBPM());

		//cout << "Temperature: " << t.getCelsius() << "°C" << endl;
		//cout << "Lightsensor: " << l.getLux() << " Lux" << endl;
		//cout << "Pulsesensor: " << p.getBPM() << " (bpm) " << "is reading?" << p.isReadingBPM() << endl;
		//cout << "------------------------------------------" << endl;
        this_thread::sleep_for(milliseconds(1000));
    }
    //p.stopReading();
}


