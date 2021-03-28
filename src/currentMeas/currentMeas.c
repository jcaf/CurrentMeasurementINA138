/*
 * currentMeas.c
 * INA138 + ADS1115 + TXRX 6N137M
 *
 * La salida del INA138 viene multiplicada por la ganancia que da el Resistor de 49.9K (G=10)
 * a 500mA * 0.05 Ohm = 0.025V -> 0.025 * G = 0.250V --> P_GAIN del ADS115 = 0.256V
 *  Created on: Mar 27, 2021
 *      Author: jcaf
 */


#include "../types.h"
#include "../ads1115/ads1115.h"
#include "../i2c/I2CCommonFx.h"
#define P_GAIN (0.256f/32768)

#define CURRENT_DEVIATION 0.0101//mV

#define IsRs 5.6e-3f
#define Is_medida 107.2e-3f
#define RSHUNT IsRs/Is_medida
#define GM 200E-6f
#define RL 49.9E3f


float currentMeas(void)
{
    int16_t ib16;
    uint8_t reg[2];
    //
    float current=0;
char b[10];
char str[20];

    I2Ccfx_ReadRegistersAtAddress(ADS115_ADR_GND, ADS1115_CONVRS_REG, &reg[0], 2);
    ib16 = (reg[0]<<8) + reg[1];
    //
    //itoa(ib16, b, 10);
    //
    current = (ib16 * P_GAIN) + 0.0101;
    //
    //dtostrf(current, 0, 5, str);
    //usart_println_string(str);

    current/=(RSHUNT*GM*RL);

    //dtostrf(current, 0, 5, str);
	//usart_println_string(str);

	return current;
}