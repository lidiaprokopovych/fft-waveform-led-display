#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init(unsigned int ubrr);

unsigned char USART_Wait_To_Receive(void);


