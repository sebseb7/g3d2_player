#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>

#include "main.h"
#include <string.h>
#include<sys/time.h>



#define FRAMETIME 33

int sdlpause = 0;

#define MAX_ANIMATIONS 200
#define MAX_APPS 200

int animationcount = 0;
int appcount = 0;

struct animation {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	int duration;
	int min_delay;
} animations[MAX_ANIMATIONS];


struct app {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	int min_delay;
} apps[MAX_APPS];


SDL_Surface* screen;

uint8_t leds[LED_HEIGHT][LED_WIDTH][2];
void setLedXY(uint8_t x, uint8_t y, uint8_t green) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	leds[y][x][0] = green;
	leds[y][x][1] = 1;
}

void invLedXY(uint8_t x, uint8_t y) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	leds[y][x][0] = 255 - leds[y][x][0];
	leds[y][x][1] = 1;
}
void getLedXY(uint8_t x, uint8_t y,uint8_t* green) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	*green = leds[y][x][0];
}

void registerAnimation(init_fun init,tick_fun tick, deinit_fun deinit,uint16_t t, uint16_t count)
{
	if(animationcount == MAX_ANIMATIONS)
		return;
	animations[animationcount].init_fp = init;
	animations[animationcount].tick_fp = tick;
	animations[animationcount].deinit_fp = deinit;
	animations[animationcount].duration = count;
	animations[animationcount].min_delay = t;

	animationcount++;

}

static uint16_t key_press;
uint16_t get_key_press( uint16_t key_mask )
{
	key_mask &= key_press;                          // read key(s)
	key_press ^= key_mask;                          // clear key(s)
	return key_mask;
}


void fillG(int8_t g)
{
	int x, y;

	for(x = 0; x < LED_WIDTH; x++) {
		for(y = 0; y < LED_HEIGHT; y++) {
			leds[y][x][0]=g;
			leds[y][x][1]=1;
		}
	}
}


int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) {

	fillG(0);

	srand(time(NULL));


	int current_animation = 0;

	screen = SDL_SetVideoMode(LED_WIDTH*ZOOM,LED_HEIGHT*ZOOM,32, SDL_SWSURFACE | SDL_DOUBLEBUF);


	animations[current_animation].init_fp();
	
	int tick_count = 0;
	int running = 1;
	//unsigned long long int startTime = get_clock();
	Uint32 lastFrame = SDL_GetTicks(); 

	while(running) {
		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			switch(ev.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYUP:
					break;
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
						case SDLK_SPACE:
							if(sdlpause == 0)
							{
								sdlpause = 1;
							}
							else{
								sdlpause = 0;
							}
							break;
						case SDLK_1:
							key_press |= KEY_ESC;
							break;
						case SDLK_2:
							key_press |= KEY_B;
							break;
						case SDLK_3:
							key_press |= KEY_A;
							break;
						case SDLK_4:
							key_press |= KEY_STICK;
							break;
							
						default: break;
					}
				default: break;
			}
		}

		animations[current_animation].tick_fp();


	


		
		int x, y;
		for(x = 0; x < LED_WIDTH; x++) {
			for(y = 0; y < LED_HEIGHT; y++) {

				if(leds[y][x][1] == 1)
				{

					SDL_Rect rect = { x*ZOOM, y*ZOOM, ZOOM,ZOOM };
					SDL_FillRect(
						screen, 
						&rect, 
						SDL_MapRGB(screen->format, 0,(uint8_t)(leds[y][x][0]<<4),0)
					);
					leds[y][x][1] = 0;

				}

			}
		}

		
		SDL_Flip(screen);


		Uint32 now = SDL_GetTicks(); 

		if( (now - lastFrame) < FRAMETIME )
		{
			SDL_Delay(FRAMETIME - (now - lastFrame));
		}
		lastFrame = SDL_GetTicks();

		
		tick_count++;


		if(tick_count == animations[current_animation].duration)
		{
			animations[current_animation].deinit_fp();

			current_animation++;
			if(current_animation == animationcount)
			{
				current_animation = 0;
			}
			tick_count=0;
	
			fillG(0);

			animations[current_animation].init_fp();


		}
	}

	SDL_Quit();
	return 0;
}

