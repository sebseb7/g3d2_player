#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/math.h"

static uint16_t a = 0;

static uint8_t tick(void) {

	float x0 = sini(a*45)/(0xffff/(float)LED_WIDTH);
	float y0 = sini((a*81)+0x1000)/(0xffff/(float)LED_HEIGHT);
	float x1 = sini(a*62)/(0xffff/(float)LED_WIDTH);
	float y1 = sini((a*36)+0x1000)/(0xffff/(float)LED_HEIGHT);
	uint8_t x, y;

	for(y = 0; y < LED_HEIGHT; y++) 
	{
		for(x = 0; x < LED_WIDTH; x++) 
		{
			int dist = sini(pythagorasf(x0-x,y0-y)*1548)>>9;
			int dist2 = sini(pythagorasf(x1-x,y1-y)*1548)>>9 ;
			setLedXY(
				x,y,
				sini((dist*dist2)+a*300)>>12
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
	registerAnimation(init,tick,deinit, 4, 2000);
}




