#include "serial.h"
#include "adc.h"
#include "i2c.h"
#include <stdint.h>
#include <stdio.h>

uint8_t twc_buffer[2];



void printer(uint8_t nr){
    if (nr == 0){return;}
    char result[6];
    sprintf(result,"%i\n\r", nr);
    serialWrite(result);
}

void read_measurement(uint8_t reg){
    uint8_t measurement[3] = {0,0,0};
    //Set forced mode and all sensors 1x
    twc_start((TWCStruct){0b11101100,Start,Transmit, &reg,1,0});
    twc_start((TWCStruct){0b11101101,Start,Recive,measurement,3,0});
    while (twc_status());
    uint16_t com = (measurement[1] << 8) | measurement[2];
    char result[10];
    sprintf(result, "%u\n\r", com);
    serialWrite(result);
}

void new_measurement(){
    //Set forced mode and all sensors 1x
    uint8_t send[2] = {0xF4, 0b00100101};
    twc_start((TWCStruct){0b11101100,Start,Transmit,send,2,0});
    while (twc_status());
}


int main (void){
    serialInit();
    twc_init();
    serialWrite("B\n\r");
    _delay_ms(500);
    uint8_t send[2] = {0xF5, 0b00000000};
    twc_start((TWCStruct){0b11101100,Start,Transmit,send,2,0});
    while(1){
	new_measurement();
	_delay_ms(1000);
	serialWrite("P");
	read_measurement(0xF7);
	serialWrite("T");
	read_measurement(0xFA);
    }

}
/*
void new_measurement(){
    //Set forced mode and all sensors 1x
    uint8_t send[2] = {0xF4, 0b00100101};
    printer(TWC_MT(0b11101100,send,2));
}

void read_measurement(uint8_t reg){
    uint8_t measurement[3] = {0,0,0};
    //Set forced mode and all sensors 1x
    printer(TWC_MT(0b11101100,&reg,1));
    printer(TWC_MR(0b11101101,measurement,3));
    uint16_t com = (measurement[1] << 8) | measurement[2];
    char result[10];
    sprintf(result, "%u\n\r", com);
    serialWrite(result);
}

void read_reg(uint8_t reg){
    uint8_t measurement;
    //Set forced mode and all sensors 1x
    printer(TWC_MT(0b11101100,&reg,1));
    printer(TWC_MR(0b11101101,&measurement,1));
    char result[6];
    sprintf(result, "%u\n\r", measurement);
    serialWrite(result);
}


int main(void){
    initTWC();
    serialInit();
    _delay_ms(500);
    serialWrite("B\n\r");

    //disable timer, filter and SPI mode
    uint8_t send[2] = {0xF5, 0b0000000};
    printer(TWC_MT(0b11101100,send,2));
    read_reg(0xD0);
    while(1){
	new_measurement();
	_delay_ms(1000);
	serialWrite("P");
	read_measurement(0xF7);
	read_reg(0xF7);
	read_reg(0xF8);
	serialWrite("T");
	read_measurement(0xFA);
    }
}
*/

