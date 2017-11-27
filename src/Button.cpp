#include <iostream> 
#include <string> 
#include <thread> 
#include <ctime> 
#include <chrono>
#include <cmath>
#include <map>
//includes for SPI/ADC
#include <wiringPi.h> 

#define BUTTON_COUNT 5 //Amount of maximum Buttons in system
#define DEBOUNCE_DELAY 200 //millisec


using namespace std;
using namespace std::chrono;

class Button {
	public:
	Button(void);
	~Button(void);
	void registerCallback(int button_pin, void(* callback)(int));
	void stopListening(void);
	
	
	private:
	void buttonLoop(int);

	map<int, void (*) (int)> pinCallbackMap;
	thread loops[BUTTON_COUNT];
	int loopsIndex=0;

};

Button::Button(){
	//clear thread-array:
	for(int i=0;i<BUTTON_COUNT;i++){
		loops[i]=thread t;
	}
}

Button::~Button(){
	stopListening();
}

void Button::stopListening(void){
	for(int i=0;i<BUTTON_COUNT;i++){
		
			loops[i].detach();
		
	}
}

void Button::buttonLoop(int button_pin){
	
	//set pinmode input
	pinMode(button_pin, INPUT);
	while(true){
		if(digitalRead(button_pin) == LOW){
			// call callback:
			pinCallbackMap[button_pin](button_pin);
			//debounce-delay:
			this_thread::sleep_for(milliseconds(DEBOUNCE_DELAY));
		}
		
	}
	
}

void Button::registerCallback(int button_pin, void(* callback)(int)){
	//add to map:
	pinCallbackMap[button_pin] = callback;
	
	if(loopsIndex < BUTTON_COUNT){
		loops[loopsIndex] = thread( & Button::buttonLoop(&button_pin), this);
		loopsIndex++;
	}
	
}



	