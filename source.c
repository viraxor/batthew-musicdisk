#include <SDL2/SDL.h> // hello
#include <SDL2/SDL_image.h> // code by Viraxor/The Noodlers
#include <bass.h> // I have never coded in C before
#include <stdio.h> // sorry in advance
#include <string.h> // 2023-2024 lol

void printError() {
	{
		const char *error = SDL_GetError();
		printf("%s\n", error);
	} // minimum scope in order to define const again next time
}

int running = 1;
SDL_Window* appWindow = NULL;
SDL_Surface* appSurface = NULL;
SDL_Renderer* appRenderer = NULL;
SDL_Point size;

char musicPathArray[5][40] = {".\\songs\\01-batthew_theme.xm",
							  ".\\songs\\02-wiff-waff.xm",
							  ".\\songs\\03-antarctica_from_space.it",
							  ".\\songs\\04-sweet_little_bombons.xm",
							  ".\\songs\\05-rough_start.xm"};
int activeTrack = 0;
int playing = 0;

int res;

SDL_Texture* unloadedTracks[5];
SDL_Texture* tracks[5];
SDL_Texture* loadedTracks[5];
SDL_Rect trackRects[5];

HMUSIC song;

SDL_Rect nowPlayingRect;

void switchSong(int trackNum) {
	BASS_MusicFree(song);
	song = BASS_MusicLoad(FALSE, musicPathArray[trackNum], 0, 0, BASS_MUSIC_SINCINTER, 0);
	tracks[activeTrack] = unloadedTracks[activeTrack];
	activeTrack = trackNum;
	tracks[trackNum] = loadedTracks[trackNum];
	nowPlayingRect.w = trackRects[activeTrack].w;
	nowPlayingRect.h = trackRects[activeTrack].h;
}

void playSong() {
	BASS_ChannelPlay(song, 0);
}

void pauseSong() {
	BASS_ChannelPause(song); 
} 

void stopSong() {
	BASS_ChannelPlay(song, 1);
	BASS_ChannelStop(song);
}

int main(int argc, char* argv[]) {
	// I had to put initialization here because the renderer overwrote to NULL if I did it in a special function??? wtf???
	int videoerror = SDL_Init(SDL_INIT_VIDEO);
	if (videoerror < 0) printError();
	SDL_Window* appWindow = SDL_CreateWindow("the gct musicdisk", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
	
	int imgerror = IMG_Init(IMG_INIT_PNG);
	if (imgerror < 0) printError();
	
	BASS_Init(-1, 48000, BASS_DEVICE_16BITS || BASS_DEVICE_STEREO, 0, NULL);
	
	SDL_Renderer* appRenderer = SDL_CreateRenderer(appWindow, -1, SDL_RENDERER_SOFTWARE);
	
	// loading assets
	SDL_Texture* batthew = IMG_LoadTexture(appRenderer, ".\\assets\\batthew.png");
	SDL_Texture* playingBar = IMG_LoadTexture(appRenderer, ".\\assets\\playingBar.png");
	SDL_Texture* tracklist = IMG_LoadTexture(appRenderer, ".\\assets\\tracklist.png");
	
	SDL_Texture* play = IMG_LoadTexture(appRenderer, ".\\assets\\play.png");
	SDL_Texture* pause = IMG_LoadTexture(appRenderer, ".\\assets\\pause.png");
	SDL_Texture* playPause = play;
	
	SDL_Texture* stop = IMG_LoadTexture(appRenderer, ".\\assets\\stop.png");
	SDL_Texture* skipFwd = IMG_LoadTexture(appRenderer, ".\\assets\\skipFwd.png");
	SDL_Texture* skipBack = IMG_LoadTexture(appRenderer, ".\\assets\\skipBack.png");
	
	for (int i = 1; i < 6; i++) {
		char buf[25];
		snprintf(buf, 20, ".\\assets\\track%d.png", i);
		
		unloadedTracks[i-1] = IMG_LoadTexture(appRenderer, buf);
		tracks[i-1] = IMG_LoadTexture(appRenderer, buf);
		snprintf(buf, 25, ".\\assets\\track%don.png", i);
		loadedTracks[i-1] = IMG_LoadTexture(appRenderer, buf);
		
		trackRects[i-1].x = 10;
		trackRects[i-1].y = 28*(i-1) + 150;
		
		SDL_QueryTexture(tracks[i-1], NULL, NULL, &size.x, &size.y);
		trackRects[i-1].w = size.x;
		trackRects[i-1].h = size.y;
	}
	
	// get rekt
	SDL_Rect playingBarRect;
	playingBarRect.y = 602;
	playingBarRect.x = 0;
	playingBarRect.w = 800;
	playingBarRect.h = 198;
	
	SDL_Rect tracklistRect;
	tracklistRect.x = 0;
	tracklistRect.y = 0;
	tracklistRect.w = 296;
	tracklistRect.h = 304;
	
	SDL_Rect playRect;
	playRect.x = 15;
	playRect.y = 725;
	playRect.w = 64;
	playRect.h = 64;
	
	SDL_Rect stopRect;
	stopRect.x = 85;
	stopRect.y = 725;
	stopRect.w = 64;
	stopRect.h = 64;
	
	SDL_Rect skipBackRect;
	skipBackRect.x = 155;
	skipBackRect.y = 725;
	skipBackRect.w = 64;
	skipBackRect.h = 64;
	
	SDL_Rect skipFwdRect;
	skipFwdRect.x = 225;
	skipFwdRect.y = 725;
	skipFwdRect.w = 64;
	skipFwdRect.h = 64;
	
	nowPlayingRect.x = 170;
	nowPlayingRect.y = 615;
	nowPlayingRect.w = trackRects[activeTrack].w;
	nowPlayingRect.h = trackRects[activeTrack].h;
	
	switchSong(activeTrack);

	// actual stuff
	while (running == 1) {
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				running = 0;
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				int x, y;
				SDL_GetMouseState(&x, &y); // need to do it like this, the event doesn't pass x&y
				SDL_Point mousePos = {x, y};
				if (SDL_PointInRect(&mousePos, &playRect)) {
					switch (playing) { // first switch I've ever wrote
						case 0: 
							playSong();
							playPause = pause;
							playing = 1;
							break;
						case 1: 
							pauseSong();
							playPause = play;
							playing = 0;
							break;
					}
				} else if (SDL_PointInRect(&mousePos, &stopRect)) {
					stopSong();
					playPause = play;
					playing = 0;
				} else if (SDL_PointInRect(&mousePos, &skipBackRect)) {
					stopSong();
					switchSong(activeTrack-1);
					if (playing == 1) playSong();
				} else if (SDL_PointInRect(&mousePos, &skipFwdRect)) {
					stopSong();
					switchSong(activeTrack+1);
					if (playing == 1) playSong();
				}
			}
		}
		
		if (BASS_ChannelIsActive(song) == BASS_ACTIVE_STOPPED && playing == 1) {
			switchSong(activeTrack+1);
			playSong();
		}
		
		SDL_RenderClear(appRenderer);
		
		SDL_RenderCopy(appRenderer, batthew, NULL, NULL);
		SDL_RenderCopy(appRenderer, playingBar, NULL, &playingBarRect);
		SDL_RenderCopy(appRenderer, tracklist, NULL, &tracklistRect);
		for (int i = 0; i < 5; i++) {
			res = SDL_RenderCopy(appRenderer, tracks[i], NULL, &trackRects[i]);
			if (res < 0) printError();
		}
		
		SDL_RenderCopy(appRenderer, playPause, NULL, &playRect);
		SDL_RenderCopy(appRenderer, stop, NULL, &stopRect);
		SDL_RenderCopy(appRenderer, skipFwd, NULL, &skipFwdRect);
		SDL_RenderCopy(appRenderer, skipBack, NULL, &skipBackRect);
		
		SDL_RenderCopy(appRenderer, tracks[activeTrack], NULL, &nowPlayingRect);
		
		SDL_RenderPresent(appRenderer);
		
		SDL_Delay(16); // about 60fps
	}
	
	// blow away
	SDL_DestroyWindow(appWindow);
	appWindow = NULL;

	IMG_Quit();
	SDL_Quit();
	
	return 0;
}