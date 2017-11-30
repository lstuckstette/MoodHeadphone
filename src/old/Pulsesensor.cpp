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

class Pulse {
    public:
        Pulse(int);
		~Pulse(void);
		void startReading(void);
		void stopReading(void);
		int getBPM(void);
    private:
		thread loop;
		int BPM = 0;
		int channel;
		bool QS = false;
		void BPMLoop(void);

};

void Pulse::BPMLoop(void) {
	// init variables
	int Signal;                // holds the incoming raw data
	int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
	bool Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
	QS = false;        // becomes true when Arduoino finds a beat.

	
    int rate[10]; // array to hold last ten IBI values
    unsigned long sampleCounter = 0; // used to determine pulse timing
    unsigned long lastBeatTime = 0; // used to find IBI
    int P = 512; // used to find peak in pulse wave, seeded
    int T = 512; // used to find trough in pulse wave, seeded
    int thresh = 525; // used to find instant moment of heart beat, seeded
    int amp = 100; // used to hold amplitude of pulse waveform, seeded
    bool firstBeat = true; // used to seed rate array so we startup with reasonable BPM
    bool secondBeat = false; // used to seed rate array so we startup with reasonable BPM
	
	//init wiring:
	
	
    steady_clock::time_point lastTime = steady_clock::now();

    while (true) {
        Signal = analogRead(ADC_BASE + channel);
		
		//cout << "sig: " << Signal  << " with base: " <<ADC_BASE << "and channel: " << channel<< endl;
		
        steady_clock::time_point currentTime = steady_clock::now();
		
		int diff = duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
		
        sampleCounter += diff;
		
		
		
        lastTime = currentTime;

        int N = sampleCounter - lastBeatTime; // monitor the time since the last beat to avoid noise
		

        //  find the peak and trough of the pulse wave
        if (Signal < thresh && N > (IBI / 5) * 3) { // avoid dichrotic noise by waiting 3/5 of last IBI
            if (Signal < T) { // T is the trough
                T = Signal; // keep track of lowest point in pulse wave
            }
			
        }

        if (Signal > thresh && Signal > P) { // thresh condition helps avoid noise
            P = Signal; // P is the peak
			
        } // keep track of highest point in pulse wave

        //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
        // signal surges up in value every time there is a pulse
        if (N > 250) { // avoid high frequency noise
            if ((Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3)) {
                Pulse = true; // set the Pulse flag when we think there is a pulse
                IBI = sampleCounter - lastBeatTime; // measure time between beats in mS
                lastBeatTime = sampleCounter; // keep track of time for next pulse

                if (secondBeat) { // if this is the second beat, if secondBeat == TRUE
                    secondBeat = false; // clear secondBeat flag
                    for (int i = 0; i <= 9; i++) { // seed the running total to get a realisitic BPM at startup
                        rate[i] = IBI;
                    }
                }

                if (firstBeat) { // if it's the first time we found a beat, if firstBeat == TRUE
                    firstBeat = false; // clear firstBeat flag
                    secondBeat = true; // set the second beat flag
                    continue; // IBI value is unreliable so discard it
                }

                // keep a running total of the last 10 IBI values
                unsigned int runningTotal = 0; // clear the runningTotal variable

                for (int i = 0; i <= 8; i++) { // shift data in the rate array
                    rate[i] = rate[i + 1]; // and drop the oldest IBI value
                    runningTotal += rate[i]; // add up the 9 oldest IBI values
                }

                rate[9] = IBI; // add the latest IBI to the rate array
                runningTotal += rate[9]; // add the latest IBI to runningTotal
                runningTotal /= 10; // average the last 10 IBI values
                BPM = 60000 / runningTotal; // how many beats can fit into a minute? that's BPM!
                QS = true; // set Quantified Self flag
                // QS FLAG IS NOT CLEARED INSIDE THIS ISR
				//cout << "reached1" << endl;
            }
        }

        if (Signal < thresh && Pulse == true) { // when the values are going down, the beat is over
            Pulse = false; // reset the Pulse flag so we can do it again
            amp = P - T; // get amplitude of the pulse wave
            thresh = amp / 2 + T; // set thresh at 50% of the amplitude
            P = thresh; // reset these for next time
            T = thresh;
			//cout << "reached2" << endl;
        }

        if (N > 2500) { // if 2.5 seconds go by without a beat
            thresh = 512; // set thresh default
            P = 512; // set P default
            T = 512; // set T default
            lastBeatTime = sampleCounter; // bring the lastBeatTime up to date
            firstBeat = true; // set these to avoid noise
            secondBeat = false; // when we get the heartbeat back
			BPM = 0;
			//cout << "reached3" << endl;
        }

        delay(5);
    }
}

Pulse::Pulse(int adc_channel) {
    //constructor
    channel = adc_channel;
}

Pulse::~Pulse(void) {
    //clean memeory
    stopReading();
}

void Pulse::startReading() {
    //init, start loop reading values;
	
    loop = thread( & Pulse::BPMLoop, this);
}

void Pulse::stopReading(void) {
    loop.detach();

}

int Pulse::getBPM(void) {
    //return current reading
    return BPM;
}