#include "Delay.h"

void TIM6_Config(void){
	/*
		 * Step to followed
		 * 1. Enable clock for timer
		 * 2. Set prescaler for timer and the ARR
		 * 3. Enable timer and wait for update flag to set
		 */

		// Timer period is 1us -> f = 1MHz. f_system = 64MHz. Choose Prescaler =

		// 1. Enable clock for timer
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

		// 2. Set prescaler for timer and the ARR
		TIM6->ARR = 0xFFFF;
		TIM6->PSC = 64 - 1;

		// 3. Enable timer and wait for update flag to set
		TIM6->CR1 |= 1;
		while(!(TIM6->SR & 1)){} //wait for update flag set (UEV need to be updated to get new psc and arr
		TIM6->SR &= ~1; //clear flag
}

void Delay_us(uint16_t us){
	/*
	 * step to follow
	 * 1. reset counter
	 * 2. wait for the counter reach to the value
	 */

	// 1. reset counter
	TIM6->CNT = 0;
	while(TIM6->CNT < us){}
}


void Delay_ms(uint16_t ms){
	for(uint16_t i = 0; i < ms; i++){
		Delay_us(1000);
	}
}
