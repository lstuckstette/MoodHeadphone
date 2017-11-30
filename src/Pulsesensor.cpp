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
		bool isReadingBPM();
    private:
		thread loop; // thread reading pulse in background
		int channel; // ADC-Channel
		void readPulse(void); // reads single BPM value if present
		int readRaw(void); // reads raw ADC value
		void pulseTimerLoop(void); // execs 'readPulse' every 2 ms
		
		//PULSEREAD VARIABLES
		int rate[10];                    // array to hold last ten IBI values
		unsigned long sampleCounter = 0;          // used to determine pulse timing
		unsigned long lastBeatTime = 0;           // used to find IBI
		int P =2048;                      // used to find peak in pulse wave, seeded
		int T = 2048;                     // used to find trough in pulse wave, seeded
		int thresh = 2048;                // used to find instant moment of heart beat, seeded
		int amp = 410;                   // used to hold amplitude of pulse waveform, seeded
		bool firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
		bool secondBeat = false;      // used to seed rate array so we startup with reasonable BPM

		// these variables are  because they are used during the interrupt service routine!
		int BPM = -1;                   // used to hold the pulse rate
		int Signal;                // holds the incoming raw data
		int IBI = 600;             // holds the time between beats, must be seeded! 
		bool pulse = false;     // true when pulse wave is high, false when it's low
		bool QS = false;        // becomes true when Pi finds a beat.

};

void Pulse::pulseTimerLoop(void){
	while(true){
		readPulse();
		delay(2);
	}
	
}
bool Pulse::isReadingBPM(void){
	return QS;
}

void Pulse::readPulse(void) {
  
  Signal = readRaw();              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

    //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T){                        // T is the trough
      T = Signal;                         // keep track of lowest point in pulse wave 
    }
  }

  if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
    P = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250){                                   // avoid high frequency noise
    if ( (Signal > thresh) && (pulse == false) && (N > (IBI/5)*3) ){        
      pulse = true;                               // set the Pulse flag when we think there is a pulse
                     
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                  // clear secondBeat flag
        for(int i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;                      
        }
      }

      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
		
        return;                              // IBI value is unreliable so discard it
      }   


      // keep a running total of the last 10 IBI values
      uint16_t runningTotal = 0;              // clear the runningTotal variable    

      for(int i=0; i<=8; i++){                // shift data in the rate array
        rate[i] = rate[i+1];                  // and drop the oldest IBI value 
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      QS = true;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }

  if (Signal < thresh && pulse == true){   // when the values are going down, the beat is over
               
    pulse = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500){                           // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

    
}

int Pulse::readRaw(void){
	return analogRead(ADC_BASE + channel);
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
	
    loop = thread( & Pulse::pulseTimerLoop, this);
}

void Pulse::stopReading(void) {
    loop.detach();

}

int Pulse::getBPM(void) {
    //return current reading
    return BPM;
}