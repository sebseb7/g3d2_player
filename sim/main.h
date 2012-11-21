#ifndef _MAIN_H
#define _MAIN_H

#include <stdint.h>

enum {
	LED_WIDTH = 72,
	LED_HEIGHT = 32,
	ZOOM = 15
};

#define G3D2

uint32_t getSysTick(void);

#define SIMULATOR

#define CONSTRUCTOR_ATTRIBUTES	__attribute__((constructor));

#define KEY_A (1<<0)
#define KEY_ESC (1<<1)
#define KEY_STICK (1<<2)
#define KEY_B (1<<3)

uint16_t get_key_press( uint16_t key_mask );


typedef void (*init_fun)(void);
typedef void (*deinit_fun)(void);
typedef uint8_t (*tick_fun)(void);

int sdlpause;

void registerAnimation(init_fun init,tick_fun tick,deinit_fun deinit, uint16_t t, uint16_t duration);
void setLedXY(uint8_t x,uint8_t  y, uint8_t g);
void invLedXY(uint8_t x,uint8_t  y);
void Delay(uint16_t t);
void getLedXY(uint8_t x, uint8_t y, uint8_t* green);
#endif

