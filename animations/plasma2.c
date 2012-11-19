#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/math.h"

static uint16_t a = 0;

static uint8_t tick(void) {

	
	uint8_t x, y;

	uint16_t sin1 = sini(a);
	float x0 = (float)sini(a*8)/256-64;
	float y0 = (float)sini((a*9)+0x1000)/256-64;
	float x1 = (float)sini(a*12)/128-128;
	float y1 = (float)sini((a*13)+0x1000)/128-128;
	
		
	for(y = 0; y < LED_HEIGHT; y++) 
	{
		uint16_t y_part =  sini(sin1+y*512);


		for(x = 0; x < LED_WIDTH; x++) 
		{
			
			float dist = pythagorasf(x0-x,y0-y);
			float dist2 = pythagorasf(y1-x,x1-y);


			uint16_t h = sini(sin1+x*512)+ y_part;
			h += sini(dist*500);
			h += sini(dist2*300);
			//uint16_t h = sini(sin1+x*600)+ y_part + sini(dist*500) + sini(dist2*300);
			setLedXY(
				x,y,
				h>>12
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
	registerAnimation(init,tick,deinit, 4, 1600);
}




