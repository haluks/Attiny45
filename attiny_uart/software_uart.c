/*
 * software_uart.c
 *
 * Created: 16.03.2022 15:10:12
 *  Author: haluk
 */ 
#include "software_uart.h"
#include <util/delay_basic.h>
volatile uint8_t rx_bas=0,rx_son=0;
volatile uint8_t rx_ring[UART_Rx_Boyut];
int16_t tx_delay=0;
int16_t rx_delay=0;
int16_t yarim_rx_delay=0;
int16_t stop_rx_delay=0;
volatile uint8_t veri=0;
ISR(PCINT0_vect){
	if (RX_PIN_LOW){
		_delay_loop_2(yarim_rx_delay);
		for (uint8_t i=0;i<8;i++){			
			_delay_loop_2(rx_delay);
			veri>>=1;
			//PORTB^=(1<<4);
			if (RX_PIN_HIGH){
				veri|=0x80 ;
				
				}else{
				veri&=~0x80 ;				
			}			
		}
		_delay_loop_2(stop_rx_delay);
		rx_bas=(rx_bas+1) & UART_Rx_Mask;
		rx_ring[rx_bas]=veri;
	}	
}

void uart_basla(Bd_rate_t _baud){
	cli();
	RX_HIGH;
	RX_IN;
	TX_HIGH;
	TX_OUT;
	PCMSK|=(1<<PCINT3);//pb3
	GIMSK|=(1<<PCIE);
	tx_delay = ((F_CPU / _baud) / 4)-3;
	yarim_rx_delay=(F_CPU/_baud/8)-7;
	if (yarim_rx_delay<=1){
		yarim_rx_delay=1;
	}
	rx_delay=(F_CPU/_baud/4)-4;
	if (rx_delay<=1){
		rx_delay=1;
	}
	stop_rx_delay=(F_CPU/_baud/4)-16;
	if (stop_rx_delay<=1){
		stop_rx_delay=1;
	}
	sei();
}

uint8_t uart_oku(){
	
	rx_son=(rx_son+1) & UART_Rx_Mask;
	return rx_ring[rx_son];
}
void uart_gonder(uint8_t uData){	
	_delay_loop_2(tx_delay);
	TX_LOW;
	_delay_loop_2(tx_delay);
	cli();
	for (uint8_t i=0;i<8;i++){
		if (uData&0x01){
			TX_HIGH;
			}else{
			TX_LOW;
		}
		_delay_loop_2(tx_delay);
		uData>>=1;
	}
	TX_HIGH;
	_delay_loop_2(tx_delay);
	sei();
}
void uart_dizi(const char *str){
	while(*str){
		uart_gonder(*str++);
	}
	
}
uint8_t uart_gelen(){
	
	if (rx_son==rx_bas)	return 0;
	return 1;
}
void uart_dizi_al(char *stri){
	uint8_t poz=0;
	do{
		stri[poz]=uart_oku();
		poz++;
	} while (!(rx_bas==rx_son));
	stri[poz]='\0';
}
