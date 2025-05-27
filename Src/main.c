#include <stdint.h>
#include <stm32f4xx.h>
#include <Delay.h>

void SysClockConfig(void){
	/*
	 * Step followed
	 * 1. Enabel HSE and wait HSE become ready
	 * 2. Set the power enable clock and voltage regulator
	 * 3. Config the Flash prefetch and LATENCY Related Settings
	 * 4. Config the prescaler HCLK, PCKL1, PCKL2
	 * 5. Config the main PLL
	 * 6. Enable PLL and wait for it become ready
	 * 7. Select the clock source and wait for it to be set
	 */

	// select HSE (8MHz in system_stm32f4xx.c and in schematics), expected systemclock is 64Mhz, select PLL (M=25, N=128, 	P=2)


	// 1. Enabel HSE and wait HSE become ready
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY)){}

	// 2. Set the power enable clock and voltage regulator
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	//because I just want to run at 64MHz, I choose scale 2 mode (<144MHz) to save energy
	PWR->CR &= ~(1 << 14);

	// 3. Config the Flash prefetch and LATENCY Related Settings
	FLASH->ACR = FLASH_ACR_LATENCY_2WS | FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN;

	// 4. Config the prescaler HCLK, PCKL1, PCKL2
	RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; //SET AHB PRESCALER
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; //SET APB1 PRESCALER
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; //SET APB2 PRESCALER

	//5. Config the main PLL
	RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSE | (0 << 16) | (64 << 6) | (4 << 0));

	// 6. Enable PLL and wait for it become ready
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)){}

	// 7. Select the clock source and wait for it to be set
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){}
}

void Uart2_Config(void){
	/* step to followed
	 * 1. Enable uart clock and gpio clock
	 * 2. Config the uart pins for alternate function
	 * 3. enable uart by writting UE bit in USART_CR1 register to 1
	 * 4. program M bit in USART_CR1 and define the word length
	 * 5. select the desired baud rate using USART_BRR register
	 * 6. Enable transmitter/receiver by setting the TE and RE bits in USART_CR1 register.
	 */

	// APB1 clock for uart max is 45MHz, systemclock is 64MHz, so APB1 for uart is 32MHz.

	// 1. Enable uart clock and gpio clock
	RCC->APB1ENR |= 1<<17; //enable clock for uart
	RCC->AHB1ENR |= 1<<0; //enable clock for gpioA

	// 2. Config the uart pins for alternate function
	GPIOA->MODER |= 0b1010 << 4; //turn on alternate function mode of PA2 and PA3
	GPIOA->OSPEEDR |= 0b1111 << 4; //choose high speed for PA2 and PA3

	GPIOA->AFR[0] |= 0x77 << 8; //AFRL function uart for pin2 and pin3

	// 3. enable uart by writting UE bit in USART_CR1 register to 1
	USART2->CR1 |= 0x00;
	USART2->CR1 |= 1<<13;

	// 4. program M bit in USART_CR1 and define the word length
	USART2->CR1 &= ~(1<<12); //M=0, 8bits word length

	// 5. select the desired baud rate using USART_BRR register
	//I choose baud rate is 115200bps, so 32.10^6/(8*2*115200)=17.36
	//0.36*16=5.76 --> choose 6
	USART2->BRR = (6<<0) | (17 << 4);

	// 6. Enable transmitter/receiver by setting the TE and RE bits in USART_CR1 register.
	USART2->CR1 |= 1<<2; //RE=1
	USART2->CR1 |= 1<<3; //TE=1
}

void USART2_sendChar(uint8_t c){
	/*
	 * step to followed
	 * 1. write the data to send in the UART_DR register (this clears TXE bit). repeat this for each data
	 * to be transmitted in case of single buffer.
	 * 2. After writing the last data into the UART_DR register, wait until TC=1, this indicates
	 * that the transmission of the last frame is completed.
	 */

	USART2->DR = c;
	while(!(USART2->SR & (1<<6))){}
}

void USART2_sendString(const char* string){
	while(*string){
		USART2_sendChar((uint8_t)*string++);
	}
}

uint8_t USART2_getChar(void){
	/*
	 * step to followed
	 * 1. wait for RNEX bit to set. This indicates data has been received and can be read
	 * 2. read the data from DR register, this clears RNEX bit also.
	 */
	uint8_t data;
	while(!(USART2->SR & (1<<5))){}
	data = USART2->DR;
	return data
}

int main(void)
{
	SysClockConfig();
	TIM6_Config();
	Uart2_Config();

	while(1){
		USART2_sendChar('G');
		Delay_ms(1500);
	}
}
