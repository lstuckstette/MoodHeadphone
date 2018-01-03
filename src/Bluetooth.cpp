

#include <dbus-c++/dbus.h>
#include <iostream>

/*
INFO:
- Wiki: http://wiki.apertium.org/wiki/Compiling_a_C%2B%2B_D-Bus_program
- libs:  sudo apt-get install libdbus-c++
- linking: -I /usr/include/dbus-c++-1 ; -l dbus-c++-1

*/
using namespace std;

class Bluetooth{
	public:
		Bluetooth(void);
		~Bluetooth(void);
		
	
};

Bluetooth::Bluetooth(void){
	cout << "alive!" << endl;
	
}

Bluetooth::~Bluetooth(void){
	
}