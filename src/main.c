#include "serial.h"
#include "adc.h"
#include "i2c.h"
uint8_t twc_buffer[2];


void new_measurement(){
    char result[2]; 
    //Set forced mode and all sensors 1x
    uint8_t send[2] = {0xF4, 0b00100101};
    result[0] = TWC_MT(0b11101100,send,2);
    result[1] = '\0';
    serialWrite(result);
}
void read_measurement(uint8_t reg){
    char result[2];
    char measurement[3];
    //Set forced mode and all sensors 1x
    result[0] = TWC_MT(0b11101100,&reg,1);
    result[1] = '\0';
    serialWrite(result);
    
    result[0] = TWC_MT(0b11101110,measurement,2);
    result[1] = '\0';
    serialWrite(result);

}


int main(void){
    initTWC();
    serialInit();
    serialWrite("Begin\n\r");
    char result[2];

    //disable timer, filter and SPI mode
    char send[2] = {0xF5, 0b0000000};
    result[0] = TWC_MT(0b11101100,send,2);
    result[1] = '\0';
    serialWrite(result);
    
    while(1){
        new_measurement();
	_delay_ms(1000);
	serialWrite("Pressure:\n\n\r");
	read_measurement(0xF7);
	serialWrite("Temperature:\n\n\r");
	read_measurement(0xFA);

    }
}

