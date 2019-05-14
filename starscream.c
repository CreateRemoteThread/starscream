/*
  main.c for sniffer component
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

#define BAUD 9600 // define baud
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1) // set baud rate value for UBRR

// function to initialize UART
void uart_init (void)
{
	UBRR0H = (BAUDRATE>>8); // shift the register right by 8 bits
	UBRR0L = BAUDRATE; // set baud rate
	UCSR0B|= (1<<TXEN0)|(1<<RXEN0); // enable receiver and transmitter
	UCSR0C = (3<<UCSZ00);
}

int uart_transmit(char data  )
{
	while (!( UCSR0A & (1<<UDRE0))); // wait while register is free
	UDR0 = data; // load data in the register
	return 0;
}

unsigned char uart_receive( void )
{
	while ( !(UCSR0A & (1<<RXC0)) );
	return UDR0;
}

#define MAX_COLS 21

int main(void)
{
	uart_init();
	FILE mystdio = FDEV_SETUP_STREAM(uart_transmit, uart_receive, _FDEV_SETUP_RW);
	
	stdout = &mystdio;
	stdin = &mystdio;

	/*
	PORT MAPPING
	- B1 is IN SYNC PLATE 1
	- B2 is LED OUT
	- C0 to C5 is SAMPLE IN
	- D3 and D4 are SAMPLE IN
	*/
	DDRB |= (1 << 0);
	DDRB |= (1 << 2);
	DDRB &= ~(1 << 1);
	
	DDRC = 0x00;
	DDRD |= (1 << 1);
	DDRD &= ~(1 << 3);
	DDRD &= ~(1 << 4);
	
	unsigned short readPart1 = 0x00;
	unsigned short readPart2 = 0x00;
	unsigned short readLast[MAX_COLS];
	unsigned short readCombined[MAX_COLS];
	
	int i = 0;
	
	for(i = 0; i < MAX_COLS;i++)
	{
		readLast[i] = 0xFF;
		readCombined[i] = 0xFF;
	}
	// initialize high state.
	
	while (1)
	{
		i = 0;
		while((PINB & (1 << 1)) != 0) {}; // manual sync plate 5 always
		
		PORTB &= ~(1 << 2);
		_delay_us(75);
		for(i = 0;i < MAX_COLS;i++)
		{
			PORTB |= (1 << 0);
			readPart1 = PINC & 0x3F;
			readPart2 = PIND & ((1 << 3) | (1 << 4)); // timing shit
			readCombined[i] = readPart1 | (readPart2 << 3);
			PORTB &= ~(1 << 0);
			_delay_us(130);
		}
		for(i = 0;i < MAX_COLS;i++)
		{
			if((readCombined[i] != readLast[i]) && (readCombined[i] != 0xFF))
			{
				PORTB ^= (1 << 2);
				uart_transmit((char )(i & 0xFF));
				uart_transmit((char )readCombined[i]);
				_delay_ms(25);
			}
			readLast[i] = readCombined[i];
		}
	}
}
