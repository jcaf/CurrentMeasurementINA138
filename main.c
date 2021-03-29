/*
 * INA138 + ADS1115 + TXRX 6N137M
 *
 * Atmega328P DIP
 * 16Mhz ext. crystal
 * Preserve EEPROM
 * Bodlevel 2.7V
 *
 * Fuses:
 * avrdude -c usbasp -B5 -p m328
 * avrdude -c usbasp -B10 -p m328p -U lfuse:w:0xff:m -U hfuse:w:0xd7:m
 * avrdude -c usbasp -B10 -p m328p -U efuse:w:0xfd:m (2.7V browout)
 *
 * proteger flash (modo 3): lectura y escritura
 * avrdude -c usbasp -B10 -p m328p -U lock:w:0xFC:m
 * https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p
 * main.c
 *
 *  Created on: Mar 27, 2021
 *      Author: jcaf
 */

#include <avr/io.h>
#include "src/system.h"
#include "src/i2c/I2C.h"
#include "src/i2c/I2CCommonFx.h"
#include "src/ads1115/ads1115.h"
#include "src/usart/usart.h"
#include "src/currentMeas/currentMeas.h"


volatile struct _isr_flag
{
    unsigned f10ms :1;
    unsigned __a :7;
} isr_flag = { 0,0 };

struct _main_flag
{
    unsigned f10ms :1;
    unsigned __a:7;

}main_flag = { 0,0 };


#define TRAMA_START '@'
#define TRAMA_END 	'\n'
void USART_txdata(const char *str);

int main(void)
{
    //ADS1115 init
    I2C_unimaster_init(100E3);//100KHz

    ////ADS1115 init set config
    uint8_t reg[2];
    reg[0] = (1<<OS_BIT) | (MUX_AIN0_GND<<MUX_BIT) | (PGA_0p256V<<PGA_BIT) | (CONTINUOUS_CONV<<MODE_BIT);
    reg[1] = (DR_8SPS<<DR_BIT);//Menor ruido
    I2Ccfx_WriteArray(ADS115_ADR_GND, ADS1115_CONFIG_REG, &reg[0], 2);

    USART_Init ( MYUBRR );

	#define NUM_SAMPLES 200
    float current = 0;
    float current_acc = 0;
    float current_media = 0;
    char current_media_str[20];
    while (1)
    {
    	current_acc=0;
    	for (int i=0; i<NUM_SAMPLES; i++)
		{
    		current = currentMeas();
    		current_acc += current;
		}
    	current_media = (current_acc/NUM_SAMPLES);
    	//
		current_media *=1000;	//expresarlo en milivoltios
		dtostrf(current_media, 0, 2, current_media_str);
		USART_txdata(current_media_str);
    }
}

void USART_txdata(const char *str)
{
	USART_Transmit(TRAMA_START);
	usart_println_string(str);
}
