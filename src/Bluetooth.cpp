#include <signal.h>
#include <sstream>
#include "Gobbledegook.h"
#include <iostream>

/*
INFO:
- Wiki: http://wiki.apertium.org/wiki/Compiling_a_C%2B%2B_D-Bus_program
- libs:  sudo apt-get install libdbus-c++
- linking: -I /usr/include/dbus-c++-1 ; -l dbus-c++-1

*/
using namespace std;




//Server data variables - holding required Values:
static int16_t serverDataTemperature = 22;
static int16_t serverDataBrightness = 1000;
static int16_t serverDataHeartrate = 66;
static std::string serverDataTextString = "ACK";

// LOGGING:
enum LogLevel
{
	Debug,
	Verbose,
	Normal,
	ErrorsOnly
};

// Our log level - defaulted to 'Normal' but can be modified via command-line options
LogLevel logLevel = Normal;

// Our full set of logging methods (we just log to stdout)
//
// NOTE: Some methods will only log if the appropriate `logLevel` is set
void LogDebug(const char *pText) { if (logLevel <= Debug) { std::cout << "  DEBUG: " << pText << std::endl; } }
void LogInfo(const char *pText) { if (logLevel <= Verbose) { std::cout << "   INFO: " << pText << std::endl; } }
void LogStatus(const char *pText) { if (logLevel <= Normal) { std::cout << " STATUS: " << pText << std::endl; } }
void LogWarn(const char *pText) { std::cout << "WARNING: " << pText << std::endl; }
void LogError(const char *pText) { std::cout << "!!ERROR: " << pText << std::endl; }
void LogFatal(const char *pText) { std::cout << "**FATAL: " << pText << std::endl; }
void LogAlways(const char *pText) { std::cout << "..Log..: " << pText << std::endl; }
void LogTrace(const char *pText) { std::cout << "-Trace-: " << pText << std::endl; }


class Bluetooth{
	public:
		Bluetooth(void);
		~Bluetooth(void);
		void setTemp(int);
		void setLight(int);
		void setHeart(int);
	private:
		void setupBluetooth();
		//const void * dataGetter(const char *pName);
		//int dataSetter(const char *, const void *);
		
		
		// Maximum time to wait for any single async process to timeout during initialization
		const int kMaxAsyncInitTimeoutMS = 30 * 1000;
};

void Bluetooth::setTemp(int t){
	serverDataTemperature = t;
	ggkNofifyUpdatedCharacteristic("/com/gobbledegook/environment/celsius");
}

void Bluetooth::setLight(int l){
	serverDataBrightness = l;
	ggkNofifyUpdatedCharacteristic("/com/gobbledegook/environment/lux");
}

void Bluetooth::setHeart(int h){
	serverDataHeartrate = h;
	ggkNofifyUpdatedCharacteristic("/com/gobbledegook/environment/heartrate");
}



const void * dataGetter(const char *pName){
	
	if (nullptr == pName)
	{
		LogError("NULL name sent to server data getter");
		return nullptr;
	}

	std::string strName = pName;

	if (strName == "temperature/degree")
	{
		return &serverDataTemperature; //degree
	}
	else if (strName == "brightness/level")
	{
		return &serverDataBrightness; //lux
	}
	else if (strName == "heartrate/bpm")
	{
		return &serverDataHeartrate; // heart bpm
	}
	
	else if (strName == "command/string")
	{
		return serverDataTextString.c_str(); // or "NACK" dependend on last issued command
	}
	
	

	LogWarn((std::string("Unknown name for server data getter request: '") + pName + "'").c_str());
	return nullptr;
}

// Called by the server when it wants to update a named value
//
// This method conforms to `GGKServerDataSetter` and is passed to the server via our call to `ggkStart()`.
//
// The server calls this method from its own thread, so we must ensure our implementation is thread-safe. In our case, we're simply
// sending over stored values, so we don't need to take any additional steps to ensure thread-safety.
int dataSetter(const char *pName, const void *pData){
	if (nullptr == pName)
	{
		LogError("NULL name sent to server data setter");
		return 0;
	}
	if (nullptr == pData)
	{
		LogError("NULL pData sent to server data setter");
		return 0;
	}

	std::string strName = pName;

	if (strName == "command/string")
	{
		//PARSE/CONSUME COMMAND AND EXECUTE!
		return 1;
	}

	LogWarn((std::string("Unknown name for server data setter request: '") + pName + "'").c_str());

	return 0;
}


void signalHandler(int signum)
{
	switch (signum)
	{
		case SIGINT:
			LogStatus("SIGINT recieved, shutting down");
			ggkTriggerShutdown();
			break;
		case SIGTERM:
			LogStatus("SIGTERM recieved, shutting down");
			ggkTriggerShutdown();
			break;
	}
}

void Bluetooth::setupBluetooth(){
	//Set logging level
	logLevel = Debug;
	
	// Register our loggers
	ggkLogRegisterDebug(LogDebug);
	ggkLogRegisterInfo(LogInfo);
	ggkLogRegisterStatus(LogStatus);
	ggkLogRegisterWarn(LogWarn);
	ggkLogRegisterError(LogError);
	ggkLogRegisterFatal(LogFatal);
	ggkLogRegisterAlways(LogAlways);
	ggkLogRegisterTrace(LogTrace);
	
	// Setup our signal handlers
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	
	// Start the server's ascync processing
	//
	// This starts the server on a thread and begins the initialization process
	//
	// !!!IMPORTANT!!!
	//
	//     This first parameter (the service name) must match tha name configured in the D-Bus permissions. See the Readme.md file
	//     for more information.
	//
	if (!ggkStart("gobbledegook", "MoodHeadphon Alpha", "MH", dataGetter, dataSetter, kMaxAsyncInitTimeoutMS))
	{
		cerr << "BT-ERROR!!!!!" << endl;
	}
	
	// Wait for the server to come to a complete stop (CTRL-C from the command line)
	//if (!ggkWait())
	//{
	//	return -1;
	//}
}


Bluetooth::Bluetooth(void){
	cout << "BT alive!" << endl;
	setupBluetooth();
	
}

Bluetooth::~Bluetooth(void){
	//KILL EVERYTHING!!!
	ggkTriggerShutdown();
}