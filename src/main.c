#include "serial.h"
#include "adc.h"
#include "i2c.h"

uint8_t twc_buffer[2];

void main(void){
    serialInit();
    char result[2];
    uint8_t send[2] = {0xF4,0x80};
    result[0] = initTWC_MT(0b11101100,send,2);
    result[1] = '\0';
    serialWrite(result);
    while(1){
        _delay_ms(500);
	serialWrite("SDA\n\r");
    }
}

