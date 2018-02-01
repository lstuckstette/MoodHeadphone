#include <signal.h>
#include <sstream>
#include "Gobbledegook.h"
#include <iostream>
#include <vector>
#include <iterator>


/*
INFO:
- Wiki: http://wiki.apertium.org/wiki/Compiling_a_C%2B%2B_D-Bus_program
- libs:  sudo apt-get install libdbus-c++
- linking: -I /usr/include/dbus-c++-1 ; -l dbus-c++-1

*/
using namespace std;




//Server data variables - holding required Values:
static int16_t serverDataTemperature = 0;
static int16_t serverDataBrightness = 0;
static int16_t serverDataHeartrate = 00;
static std::string serverDataTextString = MoodHandler::getCurrentMoodString();

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


void splitString(const std::string& str, vector<std::string>& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

void parseCommand(std::string command){
	std::vector<std::string> tokens;
	splitString(command, tokens);

	if(tokens[0] != "COMMAND"){
		std::cerr << "MALFORMED COMMAND!" << std::endl;
		return;
	}
	//COMMAND WLAN "SSID:PASSWORD"
	if(tokens[1] == "WLAN"){
		vector<std::string> wlanData;
		//replace '"' of wlanDATA:
		tokens[2] = tokens[2].substr(1,tokens[2].size()-1);
		splitString(tokens[2],wlanData,':');
		if(wlanData.size() < 2){
			std::cerr << "MALFORMED WLAN COMMAND!" << std::endl;
			return;
		}
		std::string ssid = wlanData[0];
		std::string password = wlanData[1];
		std::string command = "echo 'network={\n\tssid=\""+ssid
							+ "\"\n\tpsk=\""+password+"\n}\n\n' >> "
							+"/etc/wpa_supplicant/wpa_supplicant.conf";
		std::cout << command << std::endl;
		std::system(command.c_str());
		return;
	}
	//COMMAND SET_SPOTIFY "USERNAME:PASSWORD" -> ignore
	//COMMAND MEDIA_TOGGLE
	if(tokens[1] == "MEDIA_TOGGLE"){
		MediaPlayer::tooglePlay();
		return;
	}
	//COMMAND MEDIA_NEXT
	if(tokens[1] == "MEDIA_NEXT"){
		MediaPlayer::nextSong();
		return;
	}
	//COMMAND MEDIA_PREVIOUS
	if(tokens[1] == "MEDIA_PREVIOUS"){
		MediaPlayer::previousSong();
		return;
	}
	//COMMAND VOLUME_UP
	if(tokens[1] == "VOLUME_UP"){
		MediaPlayer::increaseLoudness();
		return;
	}
	//COMMAND VOLUME_DOWN
	if(tokens[1] == "VOLUME_DOWN"){
		MediaPlayer::decreaseLoudness();
		return;
	}
	//COMMAND COMMAND GET_SONG_STATUS
	if(tokens[1] == "COMMAND GET_SONG_STATUS"){
		serverDataTextString = MediaPlayer::getCurrentSong();
		return;
	}
	//COMMAND GET_CURRENT_MOOD
	if(tokens[1] == "GET_CURRENT_MOOD"){
		serverDataTextString = MoodHandler::getCurrentMoodString();
		return;
	}
	//COMMAND SET_CURRENT_MOOD DATA
	if(tokens[1] == "SET_CURRENT_MOOD"){
		//check command length:
		if(tokens.size() < 3){
			std::cerr << "MALFORMED SET_CURRENT_MOOD COMMAND!" << std::endl;
			return;
		}
		//set custom Flag -> disable automatic recogniton of mood!
		MoodHandler::customFlag = false;
		//replace '"' of DATA:
		tokens[2] = tokens[2].substr(1,tokens[2].size()-2);
		string data = tokens[2];
		if(data == "MOTIVATION"){
			MoodHandler::setCurrentMood(MOTIVATION);
			MoodHandler::setFittingPlaylist();
			return;
		}
		if(data == "ANTIMUEDIGKEIT"){
			MoodHandler::setCurrentMood(ANTIMUEDIGKEIT);
			MoodHandler::setFittingPlaylist();
			return;
		}
		if(data == "ENTSPANNEN"){
			MoodHandler::setCurrentMood(ENTSPANNEN);
			MoodHandler::setFittingPlaylist();
			return;
		}
		cerr << "UNKNOWN MOOD: " << data << endl;
		return;
		
	}
	
	std::cerr << "UNKNOWN COMMAND: " << tokens[1] << std::endl;
}




class Bluetooth{
	public:
		Bluetooth(void);
		~Bluetooth(void);
		void startBluetooth();
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
}

void Bluetooth::setLight(int l){
	serverDataBrightness = l;
}

void Bluetooth::setHeart(int h){
	serverDataHeartrate = h;
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

	if (strName == "text/string")
	{
		//PARSE/CONSUME COMMAND AND EXECUTE!
		std::string serverDataTextString = static_cast<const char *>(pData);
		LogWarn((std::string("got string: ")+ serverDataTextString).c_str());
		parseCommand(serverDataTextString);
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
	//logLevel = Info;
	
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
	//signal(SIGINT, signalHandler);
	//signal(SIGTERM, signalHandler);
	
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
	// Wait for the server to come to a complete stop (CTRL-C from the command line)
	ggkWait();
}