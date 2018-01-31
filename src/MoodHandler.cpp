#include <time.h>

enum Mood {MOTIVATION,ENTSPANNEN,ANTIMUEDIGKEIT};

class MoodHandler{
	
	public:
		static void initMoodHandler(void);
		static void setCurrentMood(Mood m);
		static Mood getCurrentMood(void);
		static string getCurrentMoodString(void);
		static Mood calculateCurrentMood(int temp,int light,int pulse);
		static void setFittingPlaylist(void);
		static Mood currentMood;	
		static bool customFlag;
};
Mood MoodHandler::currentMood = MOTIVATION;
bool MoodHandler::customFlag = false;


void MoodHandler::setCurrentMood(Mood m){
	MoodHandler::currentMood = m;
}

string MoodHandler::getCurrentMoodString(void){
	switch (MoodHandler::getCurrentMood()){
		case MOTIVATION: 
			return "MOTIVATION";
		case ENTSPANNEN: 
			return "ENTSPANNEN";
		case ANTIMUEDIGKEIT: 
			return "ANTIMUEDIGKEIT";
		default:
			return "NONE";
		};
}

Mood MoodHandler::getCurrentMood(void){
	return MoodHandler::currentMood;
}

Mood MoodHandler::calculateCurrentMood(int temp,int light,int pulse){
	//only if customFlag is not set:
	if(customFlag){
		return MoodHandler::currentMood;
	}
	
	//get current hour:
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	int hour = aTime->tm_hour;
	
	/*check MOTIVATION
	Pulse hoch (Sport) >= 140
	Licht eher hell > 600
	*/
	if(pulse > 140 && light > 600){
		MoodHandler::currentMood = MOTIVATION;
	} 	//check ENTSPANNEN 
	/*
	Höherer Puls (unruhig) >90 <140
	*/
	else if(pulse > 90 && pulse < 140){
		MoodHandler::currentMood = ENTSPANNEN;
	}
	/* check ANTIMUEDIGKEIT
	(Normaler Ruhepuls: 50-100)
	NIEDRIGER PULS ~<75 : http://www.heilpraxisnet.de/symptome/langsamer-puls-niedriger-puls.html
	Zeit vor 10:00 ; nach 21:00 Uhr
	Licht eher niedrig <500;
	Temperatur >15C
	*/
	else if(pulse < 75 && (hour <= 10 || hour >=21) && light <500 && temp >= 15){
		MoodHandler::currentMood = ANTIMUEDIGKEIT;
	}
	return MoodHandler::currentMood;
}

void MoodHandler::setFittingPlaylist(void){
	//TODO:
	switch (MoodHandler::currentMood){
		case MOTIVATION: 
			MediaPlayer::setPlaylist("DNB");
			break;
		case ENTSPANNEN: 
			MediaPlayer::setPlaylist("LOFI");
			break;
		case ANTIMUEDIGKEIT: 
			MediaPlayer::setPlaylist("Metal");
			break;
		default:
			break;
		
	};
};

