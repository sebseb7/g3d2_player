#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/math.h"

static uint16_t a = 0;

static uint8_t tick(void) {

	
	uint8_t x, y;

	for(y = 0; y < LED_HEIGHT; y++) 
	{
		uint16_t y_part =  sini(sini(a)*90+sini(y*200)+(sini(a*2)/1000));
		
		for(x = 0; x < LED_WIDTH; x++) 
		{
			uint32_t h=  y_part + sini(sini(sini(a)*70+sini(x*180))/(sini(a*2)/512));
			setLedXY(
				x,y,
				sini(h>>3)>>12
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
	registerAnimation(init,tick,deinit, 4, 160);
}




