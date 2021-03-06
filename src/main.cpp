//libs
#include <iostream> 
#include <thread> 
#include <chrono>
#include <wiringPi.h> 
#include <mcp3004.h>
//custom
#include "MediaPlayer.cpp"
#include "MoodHandler.cpp"
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
#define MOOD_READ_INTERVALL 60 //seconds

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
	 
	 //init mpc:
	 MediaPlayer::init();
}

int main(void) {
	
	
	
	
	initialize();

	
	
    Pulse p(PULSESENSOR_CHANNEL);
    p.startReading();
	Light l(LIGHTSENSOR_CHANNEL);
	l.startReading();
	Temp t(TEMPSENSOR_CHANNEL);
	t.startReading();
	
	Bluetooth b;
	
	//set init playlist:
	MoodHandler::setFittingPlaylist();
	int secondCounter= 0;
    while (true) {        
		//get new sensorData:
		int temperature = t.getCelsius();
		int light = l.getLux();
		int pulse = p.getBPM();
		
		//Update BLE-characteristics:
		b.setTemp(temperature);
		b.setLight(light);
		b.setHeart(pulse);
		
		//get new Mood Reading every MOOD_READ_INTERVALL seconds
		if( secondCounter == MOOD_READ_INTERVALL && !MoodHandler::customFlag){
			cout << "renewing Mood:" << endl;
			MoodHandler::calculateCurrentMood(temperature,light,pulse);
			MoodHandler::setFittingPlaylist();
			//reset counter:
			secondCounter = 0;
		}

		//cout << "Temperature: " << temperature << " °C" << endl;
		//cout << "Lightsensor: " << light << " Lux" << endl;
		//cout << "Pulsesensor: " << p.getBPM() << " (bpm) " << "is reading?" << p.isReadingBPM() << endl;
		//cout << "------------------------------------------" << endl;
		
		
        this_thread::sleep_for(seconds(1));
		secondCounter++;
    }
}

	/*
	Button b;
	b.registerCallback(BUTTON_1_GPIO, &MediaPlayer::increaseLoudness);
	b.registerCallback(BUTTON_2_GPIO, &MediaPlayer::nextSong);
	b.registerCallback(BUTTON_3_GPIO, &MediaPlayer::tooglePlay);
	b.registerCallback(BUTTON_4_GPIO, &MediaPlayer::previousSong);
	b.registerCallback(BUTTON_5_GPIO, &MediaPlayer::decreaseLoudness);
	*/
