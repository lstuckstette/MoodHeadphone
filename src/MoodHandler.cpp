

enum Mood {MOTIVATION,ENTSPANNEN,ANTIMUEDIGKEIT};

class MoodHandler{
	
	public:
		MoodHandler(void);
		~MoodHandler(void);
		static void setCurrentMood(Mood m);
		static Mood getCurrentMood(void);
		static Mood calculateCurrentMood(int temp,int light,int pulse);
		static void playFittingPlaylist(void);
	
	private:
		Mood currentMood = MOTIVATION;
};

void MoodHandler::setCurrentMood(Mood m){
	currentMood = m;
}

Mood MoodHandler::getCurrentMood(void){
	return currentMood;
}

Mood calculateCurrentMood(int temp,int light,int pulse){
	//TODO: calculate!
	return currentMood;
}

void MoodHandler::playFittingPlaylist(void){
	//TODO:
	switch (currentMood){
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

