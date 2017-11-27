#include <iostream> 
#include <string> 
#include <thread> 
#include <ctime> 
#include <chrono>
//includes for SPI/ADC
#include <wiringPi.h> 
#include <mcp3004.h> // works for mpc3008 as well

#define ADC_BASE 100 //Base for ADC read
/*
    Author: Lukas Stuckstette
*/

using namespace std;
using namespace std::chrono;

class DHT22 {
    public:
        DHT22(int);
		~DHT22(void);
		void startReading(void);
		void stopReading(void);
		int getTemp(void);
		int getHumid(void);
    private:
		thread loop;
		int channel;
		void tempLoop(void);

};