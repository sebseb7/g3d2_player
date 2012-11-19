#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/math.h"

static uint16_t a = 0;

static uint8_t tick(void) {

	
	float x0 = sini(a*40)/910.0f;
	float y0 = sini((a*80)+0x1000)/2048.0f;
	float x1 = sini(a*60)/910.0f;
	float y1 = sini((a*30)+0x1000)/2048.0f;
	uint8_t x, y;

	for(y = 0; y < LED_HEIGHT; y++) 
	{
		for(x = 0; x < LED_WIDTH; x++) 
		{
			int dist = sini(pythagorasf(x0-x,y0-y)*1024)>>3 ;
			int dist2 = sini(pythagorasf(x1-x,y1-y)*1024)>>3 ;
			setLedXY(
				x,y,
				sini(dist+dist2+(a*256))>>12
			);
		}
	}
	a+=1;
	if(a==0x4000)
	{
		a=0;
	}
	return 0;
}

static void init(void)
{
	a = 0;
}
static void deinit(void)
{
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, 4, 3000);
}




