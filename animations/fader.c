#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include "libs/math.h"

static uint8_t tick(void) {

	static int a = 0;

	a++;
	if(a==31)
		a=0;
		
	for(int y = 0; y < LED_HEIGHT; y++) 
	{
		for(int x = 0; x < LED_WIDTH; x++) 
		{
			setLedXY(
				x,y,abs(a-15)
			);
		}
	}
	return 0;
}

static void init(void)
{
}
static void deinit(void)
{
}



static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, 4, 256);
}




