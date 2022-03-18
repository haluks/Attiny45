/*
 * i2c_usi_master.c
 *
 * Created: 15.03.2022 14:07:45
 * Author : haluk
 */ 

#define F_CPU 8000000ul


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c_usi_master.h"

volatile usi_state_t usi_state=0;
static volatile uint8_t i2c_rx_bas=0,i2c_rx_son=0,i2c_tx_bas=0,i2c_tx_son=0,i2c_rx_len=0;
static volatile uint8_t i2c_rx_ring[I2C_Rx_Boyut];
static volatile uint8_t i2c_tx_ring[I2C_Tx_Boyut];
static volatile uint8_t Sladr_RW=0;
static volatile i2c_state_t i2c_state=I2C_READY;
volatile uint32_t timeout=0;
//volatile uint8_t adres_nack=0;

ISR(USI_START_vect){
	USICR&=~(1<<USISIE);
	USICR|=(1<<USIOIE);
	USISR=BIT_8_PULSE;
	SCL_LOW;
	while ((PINB&(1<<SCL)));
	USIDR    =  Sladr_RW;
	usi_state=read_ack;
}
ISR(USI_OVF_vect){
	switch (usi_state){
		case write_data:
		if(!(USIDR&NACK)&&(i2c_tx_son<i2c_tx_bas)){//ack
			
			usi_state=read_ack;
			SDA_OUT;
			USISR|=BIT_8_PULSE;
			USIDR=i2c_tx_ring[i2c_tx_son++];
			i2c_pulse();
		}else{//nack
			
			USICR&=~(1<<USIOIE);
			SDA_OUT;
			i2c_stop();
		}
		break;
		case read_data:
		if(!(USIDR&NACK)){//ack
			usi_state=write_ack;
			USISR|=BIT_8_PULSE;
			i2c_pulse();
			i2c_rx_ring[i2c_rx_bas++]=USIDR;
			SDA_OUT;
			if (i2c_rx_bas<i2c_rx_len){
				USIDR=0x00;
				}else{
				USIDR=0xFF;
			}
		}else{//nack
			//adres_nack=0;
			USICR&=~(1<<USIOIE);
			SDA_OUT;
			i2c_stop();
		}
		break;
		case write_ack:
		usi_state=read_data;
		USISR|=BIT_1_PULSE;
		i2c_pulse();
		SDA_IN;
		break;
		case read_ack:
		if (Sladr_RW&I2C_READ){//read
			usi_state=read_data;
			}else{//write
			usi_state=write_data;
		}
		USISR|=BIT_1_PULSE;
		SDA_IN;
		i2c_pulse();
		break;
		default:
		usi_state=read_ack;
		break;
	}
}
void i2c_init(){
	SDA_HIGH;
	SCL_HIGH;
	USI_OUT;
	USICR|=(1<<USISIE)|(1<<USIOIE)|(1<<USIWM1)|(1<<USICS1)|(1<<USICLK);
	USISR=(1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC);
	sei();
}
void i2c_start(){
	i2c_state= I2C_BUSSY;
	USICR|=(1<<USISIE);
	SCL_HIGH;
	//while (!(PINB&(1<<SCL)));
	SDA_LOW;
	SCL_LOW;
	SDA_HIGH;
	i2c_pulse();
}
void i2c_stop(){
	USIDR=0xFF;
	i2c_state= I2C_READY;
	SDA_LOW;
	SCL_HIGH;
	//while (!(PINB&(1<<SCL)));
	SDA_HIGH;
}
void i2c_pulse(){
	while( !(USISR & (1<<USIOIF)) ){
		_delay_us(4);
		USICR|=(1<<USITC);
		//while (!(PINB&(1<<SCL)));
		_delay_us(4);
		USICR|=(1<<USITC);
	}
}
void i2c_adr(uint8_t adr,i2c_cmd_t cmd){
	while (i2c_state!=I2C_READY){
		timeout++;
		if (timeout>I2C_TIMEOUT){
			timeout=0;
			return;
		}
	}
	i2c_tx_bas=0;
	Sladr_RW=((adr<<1)|cmd);
}
void i2c_data(uint8_t data){
	i2c_tx_ring[i2c_tx_bas++]=data;
}
void i2c_end(){
	i2c_tx_son=0;
	i2c_start();
}
void i2c_send_data(uint8_t adr, uint8_t data){
	i2c_adr(adr, I2C_WRITE);
	i2c_data(data);
	i2c_end();
}
void i2c_send(uint8_t adr, uint8_t* str, uint8_t len){
	i2c_adr(adr, I2C_WRITE);
	for (uint8_t i=0;i<len;i++){
		i2c_data( str[i]);
	}
	i2c_end();
}
void i2c_read(uint8_t adr, uint8_t len){
	i2c_rx_bas=0;
	i2c_rx_son=0;
	i2c_rx_len=0;
	i2c_adr(adr, I2C_READ);
	i2c_rx_len=len;
	i2c_start();
}
uint8_t i2c_gelen(){
	return i2c_rx_bas-i2c_rx_son;
	
}
uint8_t i2c_oku(){
	
	return i2c_rx_ring[i2c_rx_son++];
}

/*
uint8_t i2c_adres_scan(){
	for (uint8_t i=0;i<128;i++){
		adres_nack=1;
		i2c_send_data(i,0x00);
		_delay_ms(10);
		if (adres_nack)
		return i;
	}
	return 0;
}*/