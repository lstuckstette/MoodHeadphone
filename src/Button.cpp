#include <iostream> 
#include <string> 
#include <thread> 
#include <ctime> 
#include <chrono>
#include <cmath>
#include <map>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <cassert>
//includes for SPI/ADC
#include <wiringPi.h> 



#define BUTTON_COUNT 5 //Amount of maximum Buttons in system
#define DEBOUNCE_DELAY 200 //millisec


using namespace std;
using namespace std::chrono;

//typedef vor function pointers:
typedef void (*functionPointer)(void); 

struct FunctionMap{

    std::map<int,pair<functionPointer,type_index>> m1;

    template<typename T>
    void insert(int s1, T f1){
        auto tt = type_index(typeid(f1));
        m1.insert(make_pair(s1,
                        make_pair((functionPointer)f1,tt)));
    }

    template<typename T,typename... Args>
    T searchAndCall(int s1, Args&&... args){
        auto mapIter = m1.find(s1);
        /*chk if not end*/
        auto mapVal = mapIter->second;

        //auto typeCastedFun = reinterpret_cast<T(*)(Args ...)>(mapVal.first); 
		auto typeCastedFun = reinterpret_cast<functionPointer>(mapVal.first); 
        //auto typeCastedFun = (T(*)(Args ...))(mapVal.first); 

        //compare the types is equal or not
        //assert(mapVal.second == type_index(typeid(typeCastedFun)));
		//cout << "weird s1: " << s1 << endl;
		return typeCastedFun();
		//return typeCastedFun(s1);
        //return typeCastedFun(forward<Args>(args)...);
    }
	
};

class Button {
	public:
	Button(void);
	~Button(void);
	void registerCallback(int button_pin, functionPointer callback );
	void stopListening(void);
	
	
	private:
	void buttonLoop(int);

	FunctionMap pinCallbackMap;
	vector<thread> threads;


};

Button::Button(){
	
}

void Button::registerCallback(int button_pin, functionPointer callback){
	//Setup Pullup/Pulldown resistor AND pinMode for GPIO:
	pinMode(button_pin, INPUT);
	pullUpDnControl (button_pin, PUD_UP) ;
	
	//add to map:
	pinCallbackMap.insert(button_pin, callback);
	// create thread and push to vector:
	threads.push_back(thread(& Button::buttonLoop, this, button_pin));

	//thread( & Light::lightLoop, this);
}

Button::~Button(){
	stopListening();
}

void Button::stopListening(void){
	//loop through threads and kill them all!
	for(vector<thread>::iterator it = threads.begin(); it != threads.end(); ++it) {
    /* std::cout << *it; ... */
	(*it).detach();
	}
}

void Button::buttonLoop(int button_pin){
	
	while(true){

		if(digitalRead(button_pin) == LOW){

			// call callback:
			pinCallbackMap.searchAndCall<void>(button_pin,button_pin);
			
			//debounce-delay:
			this_thread::sleep_for(milliseconds(DEBOUNCE_DELAY));
		}
	}
}





	