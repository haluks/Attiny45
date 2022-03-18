/*
 * software_uart.h
 *
 * Created: 16.03.2022 15:09:54
 *  Author: haluk
 */ 


#ifndef SOFTWARE_UART_H_
#define SOFTWARE_UART_H_

#define F_CPU 8000000ul
#include <avr/io.h>
#include <avr/interrupt.h>

#define TX_DDR			DDRB
#define TX_PORT			PORTB
#define TX_PIN			PINB

#define RX_DDR			DDRB
#define RX_PORT			PORTB
#define RX_PIN			PINB

#define TX_				PB4
#define RX_				PB3
#define TX_OUT			TX_DDR|=(1<<TX_)
#define TX_HIGH			TX_PORT|=(1<<TX_)
#define TX_LOW			TX_PORT&=~(1<<TX_)
#define RX_IN			RX_DDR&=~(1<<RX_)
#define RX_HIGH			RX_PORT|=(1<<RX_)
#define RX_LOW			RX_PORT&=~(1<<RX_)
#define RX_PIN_HIGH		RX_PIN&(1<<RX_)
#define RX_PIN_LOW		!(RX_PIN&(1<<RX_))

/////////////////////////////////
#define UART_Rx_Boyut 64
#define UART_Rx_Mask (UART_Rx_Boyut-1)
typedef enum{
	Bd4800			=4800,
	Bd9600			=9600,
	Bd19200			=19200,
	Bd38400			=38400,
	Bd57600			=57600,
	Bd115200		=115200,//RX hata yüksek
	Bd230400		=230400,//ilk biti sinirda yakaliyor RX çok kötü	
}Bd_rate_t;

////////////////////////////////////////////////////
void uart_basla(Bd_rate_t baud);
uint8_t uart_oku();
void uart_gonder(uint8_t uData);
void uart_dizi(const char *str);
uint8_t uart_gelen();
void uart_dizi_al( char *stri);

#endif 