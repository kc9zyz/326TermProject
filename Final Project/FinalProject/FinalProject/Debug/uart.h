void USART_Transmit(char c, FILE *stream);
char USART_Receive(FILE *stream);

void USART_Init(unsigned int ubrr);

FILE uart_output = FDEV_SETUP_STREAM(USART_Transmit, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, USART_Receive, _FDEV_SETUP_READ);
