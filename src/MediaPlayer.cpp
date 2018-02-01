#include <iostream> 
#include <string> 
#include <fstream>

#define AMOUNT "10"

using namespace std;
using namespace std::chrono;

class MediaPlayer {
    public:
        MediaPlayer(void);
		~MediaPlayer(void);

		static void playSong(string name);
		static void setPlaylist(string name);
		static void tooglePlay(void);
		static void nextSong(void);
		static void previousSong(void);
		static void increaseLoudness(void);
		static void decreaseLoudness(void);
		static void setLoudness(int);
		static void play(void);
		static void init(void);
		static string getCurrentSong(void);
};

string MediaPlayer::getCurrentSong(void){
	system("mpc > /share/MoodCode/consolebuffer");
	string status = "";
	ifstream inFile;
	inFile.open("/share/MoodCode/consolebuffer");
	if(inFile){
		string line;
		while(inFile){
			getline(inFile,line);
			status = status+line+"\n";
			break; //only first line!
		}
	}
	return status;
}

void MediaPlayer::init(void){
	system("mpc volume 50");
	system("mpc random on");
}

void MediaPlayer::play(){
	system("mpc play");
}

void MediaPlayer::setLoudness(int amount){
	string tmp = "mpc volume " ;
	tmp +=amount;
	cout << tmp << endl;
	system(tmp.c_str());
	
}

void MediaPlayer::playSong(string title){
	system("mpc pause");
	system("mpc clear");
	string tmp = "mpc search title \"" + title +"\" | mpc add";
	cout << tmp << endl;
	system(tmp.c_str());
	system("mpc play");
}

void MediaPlayer::setPlaylist(string playlistTitle){
	system("mpc crop");
	string tmp = "mpc load \"" + playlistTitle+ "\"";
	cout << tmp << endl;
	system(tmp.c_str());
}

void MediaPlayer::tooglePlay(void){
	system("mpc toggle");
}

void MediaPlayer::nextSong(void){
	system("mpc next");
}

void MediaPlayer::previousSong(void){
	system("mpc prev");
}
void MediaPlayer::increaseLoudness(void){
	string tmp = "mpc volume +";
	tmp +=AMOUNT;
	cout << tmp << endl;
	system(tmp.c_str());
}
void MediaPlayer::decreaseLoudness(void){
	string tmp = "mpc volume -" ;
	tmp +=AMOUNT;
	cout << tmp << endl;
	system(tmp.c_str());
}

