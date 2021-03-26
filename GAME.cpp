#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <SDL.h>
#include <stdlib.h>
#include <time.h> 
#include <iostream>
#include <vector>
#include "SDL2.h"
#include "map.h"
#include "pacman.h"
#include "monster.h"

void close() {
    gSrc.free();
    gMap.free();
    whitemap.free();
    SDL_DestroyRenderer( gRenderer );
    gRenderer = NULL;
    IMG_Quit();
    SDL_Quit();
	Mix_Quit();
}

void loadMedia() {
    //Loading success flag
    gSrc.loadFromFile( "src/src.png" );
    whitemap.loadFromFile( "src/map.png" );

    //init other headers
    initmonster();
    initpacman();
    initmap();
    initwall();
    initdot();
    initdot2();
    initnumber();
    initsound();
    initbigdot();
    initway();
    initboxtrace();
}

void startgame(){
	SDL_Event e;
    int frame = 0;
    Mix_PlayMusic(sstart, -1);
	
    for(; ; ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                exit(0);
            }
        }
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 ); SDL_RenderClear( gRenderer );
        gMap.render( 0, 0 );
        pstart(gSrc);
        gSrc.render(202, 325, &READY);
        for(int i = 0; i < 4; ++i) getmonster(gSrc, i, 0);
        showpoint2(gSrc);
        SDL_RenderPresent( gRenderer );
        ++frame;
        if(frame == 252) return;
	}
}

bool gameplay() {
    SDL_Event e;
    int frame = 0;
    Mix_PlayMusic(sghost1, -1);
	
    for(; ; ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                exit(0);
            }
        }
        if(doteatean == 240 && bigdoteaten == 4) return true;
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 ); SDL_RenderClear( gRenderer );
        gMap.render( 0, 0 );
		showlive(gSrc);
		showpoint(gSrc, point);
		///just show something
        if(death) {
			if(death / 12 == 11) {
				death = 0;
				return false;
			}
			pdie(gSrc);
			++death;
			SDL_RenderPresent( gRenderer );
			++frame;
			continue;
        }
		///check if a ghost is 'being eaten'
		if(ghosteating) {
			showpoint2(gSrc);
			dot(gSrc);
			for(int i = 0; i < 4; ++i) if(scared[i]) { getmonster(gSrc, i, frame); }
			if((frame + (61 - ghosteating) % 24 < 12)) bigdot(gSrc);
			--ghosteating;
			if(!ghosteating) {
				if(beinggod) Mix_PlayMusic(chaos, -1);
				else switch (bigdoteaten) {
					case 1 : Mix_PlayMusic(sghost1, -1);
					case 2 :  Mix_PlayMusic(sghost2, -1);
					case 3 :  Mix_PlayMusic(sghost3, -1);
					case 4 :  Mix_PlayMusic(sghost4, -1);
					default : Mix_PlayMusic(sghost5, -1);
				}
			}
			SDL_RenderPresent( gRenderer );
			continue;
		}
        ///check 'beingod' pacman
		if(beinggod) {
			--beinggod;
			if(!beinggod) {
				for(int i = 0; i < 4; ++i) scared[i] = 0;
				switch (bigdoteaten) {
					case 1 : Mix_PlayMusic(sghost1, -1);
					case 2 :  Mix_PlayMusic(sghost2, -1);
					case 3 :  Mix_PlayMusic(sghost3, -1);
					case 4 :  Mix_PlayMusic(sghost4, -1);
					default : Mix_PlayMusic(sghost5, -1);
				}
			}
		}
		///usual
		scan();
		checkdot(pacx, pacy);
		checkbigdot(pacx, pacy);
		checklastmove();
		const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
		if( currentKeyStates[ SDL_SCANCODE_UP ] ) {
			changedir(3);

		} else if( currentKeyStates[ SDL_SCANCODE_DOWN ] ) {
			changedir(1);
		} else if( currentKeyStates[ SDL_SCANCODE_LEFT ] ) {
			changedir(2);
		} else if( currentKeyStates[ SDL_SCANCODE_RIGHT ] ) {
			changedir(0);
		}
		dot(gSrc);
		if(frame % 24 < 12) bigdot(gSrc);
		getpacman((frame / 4) % 4, gSrc);
		
		if(frame % 4 == 0) {
			checkdot(pacx, pacy);
			checkbigdot(pacx, pacy);
			if( currentKeyStates[ SDL_SCANCODE_UP ] ) {
				changedir(3);
			} else if( currentKeyStates[ SDL_SCANCODE_DOWN ] ) {
				changedir(1);
			} else if( currentKeyStates[ SDL_SCANCODE_LEFT ] ) {
				changedir(2);
			} else if( currentKeyStates[ SDL_SCANCODE_RIGHT ] ) {
				changedir(0);
			}
			getpacman((frame / 4) % 4, gSrc);
		}

		for(int i = 0; i < 4; ++i) if(scared[i]){ 
			if(frame % 5) mmove(i);
			getmonster(gSrc, i, frame); 
		} else {
			mmove(i);
			getmonster(gSrc, i, frame); 
			if(ms[i] && frame % 15 == 0) mmove(i);
			getmonster(gSrc, i, frame); 
		}
		unscan();
        SDL_RenderPresent( gRenderer );
        ++frame;
    }
}

void endgame2() {
	SDL_Event e;
    int frame = 0;
    for(; ; ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                exit(0);
            }
        }
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 ); SDL_RenderClear( gRenderer );
        gMap.render( 0, 0 );
        gSrc.render(185, 325, &GAMEOVER);
        showpoint2(gSrc);
        SDL_RenderPresent( gRenderer );
        ++frame;
        if(frame == 252) return;
	}
}

void endgame() {
	Mix_PauseMusic();
	SDL_Event e;
    int frame = 0;
    for(; ; ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                exit(0);
            }
        }
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 ); SDL_RenderClear( gRenderer );
        if(frame % 18 < 9) gMap.render( 0, 0 );
        else whitemap.render( 0, 0);
        pstart(gSrc);
        showpoint2(gSrc);
        SDL_RenderPresent( gRenderer );
        ++frame;
        if(frame == 252) return;
	}
}

void thegame() {
	
	for(int i = 0; i < 5; ++i) {
		reset();
		startgame();
		if(gameplay() == true) {
			endgame();
			resetmap();
		} else {
			if(lives) startgame;
			else endgame2();
			--lives;
			i = 5;
		}
//		endgame2();
	}
}
int main( int argc, char* args[] ) {
    init();
    loadMedia();
    thegame();
    close();
	return 0;
}
