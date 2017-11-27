#include <iostream>
#include <string>
#include <thread>
#include <ctime>
#include <chrono>
//includes for SPI/ADC
#include <wiringPi.h>

#define MAX_TIMINGS	85
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
    int getTemperature(void);
    int getHumidity(void);
	bool isDataReady();

private:
    thread loop;
    float temp = -1, humid = -1;
    int dht_pin;
    void tempLoop(void);
};

bool DHT22::isDataReady(){
	return temp != -1 ? true : false;
}

void DHT22::tempLoop(void)
{

    int data[5] = { 0, 0, 0, 0, 0 };

    while (true) {
		//cout << "pin "<< DHT_PIN << endl;
		
        uint8_t laststate = HIGH;
	uint8_t counter	= 0;
	uint8_t j = 0;
	uint8_t i;

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	/* pull pin down for 18 milliseconds */
	pinMode(dht_pin, OUTPUT);
	digitalWrite(dht_pin, LOW);
	delay(18);

	/* prepare to read the pin */
	pinMode(dht_pin, INPUT);

	/* detect change and read data */
	for ( i = 0; i < MAX_TIMINGS; i++ ) {
		counter = 0;
		while ( digitalRead( dht_pin ) == laststate ) {
			counter++;
			
			delayMicroseconds( 1 );
			if ( counter == 255 ) {
				break;
			}
		}
		laststate = digitalRead( dht_pin );

		if ( counter == 255 )
			break;

		/* ignore first 3 transitions */
		if ( (i >= 4) && (i % 2 == 0) ) {
			/* shove each bit into the storage bytes */
			data[j / 8] <<= 1;
			if ( counter > 16 )
				data[j / 8] |= 1;
			j++;
		}
	}

	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ( (j >= 40) && (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) ) {
		float h = (float)((data[0] << 8) + data[1]) / 10;
		if ( h > 100 ) {
			h = data[0];	// for DHT11
		}
		float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
		if ( c > 125 ) {
			c = data[2];	// for DHT11
		}
		if ( data[2] & 0x80 ) {
			c = -c;
		}
		temp = c;
		
		humid = h;
		//printf( "read_dht_data() Humidity = %.1f %% Temperature = %.1f *C \n", humid, temp );
		
	} else {
		 //printf( "read_dht_data() Data not good, skip\n" );
		//humid = temp = -1;
		
	}
		//wait at least 2 seconds until next scan:
		this_thread::sleep_for(seconds(2));
    }
}

int DHT22::getTemperature(void)
{
    return temp;
}

int DHT22::getHumidity(void)
{
    return humid;
}

DHT22::DHT22(int dhtpin)
{
    //constructor
    dht_pin = dhtpin;
}

DHT22::~DHT22(void)
{
    //clean memeory
    stopReading();
}

void DHT22::startReading()
{
    //init, start loop reading values;

    loop = thread(&DHT22::tempLoop, this);
}

void DHT22::stopReading(void)
{
    loop.detach();
}