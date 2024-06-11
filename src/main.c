#include "serial.h"
#include "adc.h"
#include "i2c.h"
#include <cstdint>

uint8_t twc_buffer[2]

void main(void){
    serialInit();
    initTWC_MT(1110100, uint8_t _data);
    while(1){
        _delay_ms(500);
	serialWrite("SDA\n\r");
    }
}

