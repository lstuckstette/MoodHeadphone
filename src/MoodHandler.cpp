

enum Mood {MOTIVATION=1,ENTSPANNEN=2,ANTIMUEDIGKEIT=3};

class MoodHandler{
	
	public:
		static void initMoodHandler(void);
		static void setCurrentMood(Mood m);
		static Mood getCurrentMood(void);
		static Mood calculateCurrentMood(int temp,int light,int pulse);
		static void playFittingPlaylist(void);
		static Mood currentMood;	
};
Mood MoodHandler::currentMood = MOTIVATION;


void MoodHandler::setCurrentMood(Mood m){
	MoodHandler::currentMood = m;
}

Mood MoodHandler::getCurrentMood(void){
	return MoodHandler::currentMood;
}

Mood MoodHandler::calculateCurrentMood(int temp,int light,int pulse){
	//TODO: calculate!
	return MoodHandler::currentMood;
}

void MoodHandler::playFittingPlaylist(void){
	//TODO:
	switch (MoodHandler::currentMood){
		case MOTIVATION: 
			MediaPlayer::playPlaylist("Metal");
			break;
		case ENTSPANNEN: 
			break;
		case ANTIMUEDIGKEIT: 
			break;
		default:
			break;
		
	};
};

