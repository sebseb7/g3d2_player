#include "main.h"

//#include "lib/usb_serial.h"

/*
 *	boot loader: http://www.st.com/stonline/stappl/st/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/APPLICATION_NOTE/CD00167594.pdf (page 31)
 *  data sheet : http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/DATASHEET/CD00277537.pdf
 *
 *
 */

static __IO uint32_t TimingDelay;
static __IO uint32_t tick;
void Delay(__IO uint32_t nTime)
{
	TimingDelay = nTime*10;

	while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
	tick++;
}


#define MAX_ANIMATIONS 30

static int animationcount;



struct animation {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	uint16_t duration;
	uint16_t timing;
};

static struct animation animations[MAX_ANIMATIONS] CCM_ATTRIBUTES;

uint8_t leds[LED_HEIGHT][LED_WIDTH] CCM_ATTRIBUTES;
void setLedXY(uint8_t x, uint8_t y, uint8_t green) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	leds[y][x] = green;
}
void getLedXY(uint8_t x, uint8_t y,uint8_t* green) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	*green = leds[y][x];
}

void fillG(int8_t green) {
	for(int x = 0; x < LED_HEIGHT;x++)
	{
		for(int y = 0;y < (LED_WIDTH>>1);y++)
		{
			leds[x][y] = green;
		}
	}
}

void sendByte(USART_TypeDef* USARTx,uint8_t byte)
{
	USART_SendData(USARTx, byte);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){};
}

void sendByteEscaped(USART_TypeDef* USARTx,uint8_t byte)
{
	switch (byte)
	{
		case 0x65:
			sendByte(USARTx,0x65);
			sendByte(USARTx,0x03);
			break;
		case 0x23:
			sendByte(USARTx,0x65);
			sendByte(USARTx,0x01);
			break;
		case 0x42:
			sendByte(USARTx,0x65);
			sendByte(USARTx,0x02);
			break;
		case 0x66:
			sendByte(USARTx,0x65);
			sendByte(USARTx,0x04);
			break;
		default:
			sendByte(USARTx,byte);
			break;
	}
}

void registerAnimation(init_fun init,tick_fun tick, deinit_fun deinit,uint16_t t, uint16_t count)
{

	// this is for initialization, probably registerAnimation gets called bevore global variables are initialized
	static int firstrun = 1;

	if(firstrun == 1)
	{
		firstrun = 0;
		animationcount = 0;
	}


	if(animationcount == MAX_ANIMATIONS)
		return;
	animations[animationcount].init_fp = init;
	animations[animationcount].tick_fp = tick;
	animations[animationcount].deinit_fp = deinit;
	animations[animationcount].duration = count;
	animations[animationcount].timing = t;

	animationcount++;
}


int main(void)
{
	RCC_ClocksTypeDef RCC_Clocks;


	for(int x = 0 ; x < LED_WIDTH;x++)
	{
		for(int y = 0; y < LED_HEIGHT; y++)
		{
			leds[y][x] = 0;
		}
	}

	RCC_GetClocksFreq(&RCC_Clocks);
	/* SysTick end of count event each 0.1ms */
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 10000);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	
	//prepare init structure
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
		// LEDs
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;       
		GPIO_Init(GPIOB, &GPIO_InitStructure);  
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;       
		GPIO_Init(GPIOB, &GPIO_InitStructure);  
	}

	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 500000;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(USART2, &USART_InitStructure);
		USART_Cmd(USART2, ENABLE);
	}
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 500000;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(UART4, &USART_InitStructure);
		USART_Cmd(UART4, ENABLE);
	}
	int current_animation = 0;
	animations[current_animation].init_fp();
	int tick_count = 0;
	

	int loopcount = 0;

	while(1)
	{
		loopcount++;
		if((loopcount == 55)||(loopcount == 57))
		{
			GPIOB->ODR           &=       ~(1<<13);
		}
		if((loopcount == 56)||(loopcount == 58))
		{
			GPIOB->ODR           |=       1<<13;
			if(loopcount==58)
				loopcount = 0;
		}

		animations[current_animation].tick_fp();

		GPIOB->ODR           |=       1<<12;

		sendByte(USART2,0x23);
		sendByte(UART4,0x23);

		for(int x = 0; x < 32;x++)
		{
			for(int y = 0;y < (LED_WIDTH>>1);y++)
			{
				sendByteEscaped(USART2, (leds[x][y*2+1]<<4) + (leds[x][y*2]&0xf)  );
			}
		}
		for(int x = 32; x < 64;x++)
		{
			for(int y = 0;y < (LED_WIDTH>>1);y++)
			{
				sendByteEscaped(UART4, (leds[x][y*2+1]<<4) + (leds[x][y*2]&0xf)  );
			}
		}

		GPIOB->ODR           &=       ~(1<<12);


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

}
