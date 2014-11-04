#include <avr/io.h>
#include <stdio.h>

void USART_Init(unsigned int ubrr) {
	/* set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;

    UCSR0C = _BV(USBS0) | _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data, 2 stop bits */ 
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */    
}

void USART_Transmit(char c, FILE *stream) {
    if (c == '\n') {
        USART_Transmit('\r', stream); /* transmit carriage return with new line */
    }
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) )
    ;
    /* Put data into buffer, sends the data */
    UDR0 = c;
}

char USART_Receive(FILE *stream) {
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
    ;
    /* Get and return received data from buffer */
    return UDR0;
}
