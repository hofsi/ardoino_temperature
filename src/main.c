#include "serial.h"
#include "adc.h"
#include <cstdint>

uint8_t twc_buffer[2]

void main(void){
    adcInit();
    serialInit();
    while(1){
        _delay_ms(500);
	serialWrite("SDA\n\r");
	adcStartConversion(0);
	_delay_ms(500);
	serialWrite("SCL\n\r");
	adcStartConversion(1);
    }
}

