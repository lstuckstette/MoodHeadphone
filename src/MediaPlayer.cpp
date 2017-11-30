#include <iostream> 
#include <string> 

#define AMOUNT "5"

using namespace std;
using namespace std::chrono;

class MediaPlayer {
    public:
        MediaPlayer(void);
		~MediaPlayer(void);

		static void playSong(string name);
		static void playPlaylist(string name);
		static void tooglePlay(void);
		static void nextSong(void);
		static void previousSong(void);
		static void increaseLoudness(void);
		static void decreaseLoudness(void);
		//string getCurrentSong(void);
};

void MediaPlayer::playSong(string title){
	system("mpc pause");
	system("mpc clear");
	string tmp = "mpc search title \"" + title +"\" | mpc add";
	cout << tmp << endl;
	system(tmp.c_str());
	system("mpc play");
}

void MediaPlayer::playPlaylist(string playlistTitle){
	system("mpc pause");
	system("mpc clear");
	string tmp = "mpc load \"" + playlistTitle+ "\"";
	cout << tmp << endl;
	system(tmp.c_str());
	system("mpc play");
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

