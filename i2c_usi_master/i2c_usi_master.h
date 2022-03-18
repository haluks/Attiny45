/*
 * i2c_usi_master.h
 *
 * Created: 15.03.2022 14:08:11
 *  Author: haluk
 */ 


#ifndef I2C_USI_MASTER_H
#define I2C_USI_MASTER_H




#define I2C_Rx_Boyut					16
#define I2C_Tx_Boyut					16
#define I2C_TIMEOUT						1000
#define NACK			0x01
#define SDA				PB0
#define SCL				PB2
#define USI_PORT		PORTB
#define USI_DDR			DDRB
#define USI_OUT			USI_DDR|=(1<<SDA)|(1<<SCL)
#define SDA_OUT			USI_DDR|=(1<<SDA)
#define SDA_IN			USI_DDR&=~(1<<SDA)

#define SCL_LOW			USI_PORT&=~(1<<SCL)
#define SCL_HIGH		USI_PORT|=(1<<SCL)
#define SDA_LOW			USI_PORT&=~(1<<SDA)
#define SDA_HIGH		USI_PORT|=(1<<SDA)

#define BIT_8_PULSE	0xF0
#define BIT_1_PULSE	0xFE

typedef enum {
	I2C_WRITE		=0,
	I2C_READ		=1,
}i2c_cmd_t;
typedef  enum {
	I2C_READY		=0,
	I2C_BUSSY		=1,
}i2c_state_t;
typedef  enum {
	write_data		=0,
	read_data		=1,
	write_ack		=2,
	read_ack		=3,
}usi_state_t;

void i2c_init();
void i2c_start();
void i2c_stop();
void i2c_pulse();
void i2c_adr(uint8_t adr,i2c_cmd_t cmd);
void i2c_data(uint8_t data);
void i2c_end();
void i2c_send_data(uint8_t adr, uint8_t datat);
void i2c_send(uint8_t adr, uint8_t* str,uint8_t len);
void i2c_read(uint8_t adr, uint8_t len);
uint8_t i2c_gelen();
uint8_t i2c_oku();
//uint8_t i2c_adres_scan();

#endif 