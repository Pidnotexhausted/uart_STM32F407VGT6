# UART REGISTER STM32F4
Use usart2 (PA2 and PA3)
- Config system clock
- Enable clock for uart and gpio (alternate function)
- Config GPIO (alternate function, high speed, AFRL function uart for pin2 and pin3)
- Enable USART2 (CR1)
- Choose M bit = 0 (8bit data word length)
- Config baud rate (more infor in SMT32F4xx ref manual - page 1013):
  + I'm using system clock 64MHz --> APB1 for uart is 32MHz.
  + Choose baud rate is 115200bps, OVER8=0 (oversampling by 16) --> USARTDIV = 32.10^6 / (8 x (2-OVER8) x baudrate) = 17.36
  + 0.36 x 16 = 5.76 --> choose 6 for DIV_Fraction.
  + choose 17 for DIV_Mantissa
  + actualy, USARTDIV = DIV_Mantissa + DIV_Fraction x 16 = 17.375
- Enable RE and TE bit in CR1
- Write a function to send 8bits:
  + write a char to DR register
  + wait for TC bit in SR register set (transmission completed)
