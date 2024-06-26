#include "serial.h"
#include "adc.h"
#include "i2c.h"
#include <stdint.h>
#include <stdio.h>

uint8_t twc_buffer[2];
uint16_t dig_T1, dig_T2, dig_T3;



int32_t compensate_T(int32_t adc_T)
{
    int32_t var1, var2;
    var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    int32_t T = ((t_fine * 5 + 128) >> 8);
    return T;
}


uint16_t get_register(uint8_t reg){
    uint8_t measurement[3] = {0,0,0};
    twc_start((TWCStruct){0b11101100,Start,Transmit, &reg,1,0});
    twc_start((TWCStruct){0b11101101,Start,Recive,measurement,3,0});
    while (twc_status());
    return (measurement[2] << 8) | measurement[1];
}

void printer(uint16_t nr){
    if (nr == 0){return;}
    char result[8];
    sprintf(result,"%i\n\r", nr);
    serialWrite(result);
}

void read_measurement(uint8_t reg, uint8_t type){
    uint8_t measurement[4] = {0,0,0, 0};
    twc_start((TWCStruct){0b11101100,Start,Transmit, &reg,1,0});
    twc_start((TWCStruct){0b11101101,Start,Recive,measurement,4,0});
    while (twc_status());
    int32_t com = ((uint32_t)measurement[1] << 12) | ((uint16_t)measurement[2] << 4) | (measurement[3] >> 4);
    if (type == 1)
	com = compensate_T((int32_t)com);
    char result[14];
    sprintf(result, "%li\n\r", com);
    serialWrite(result);
}


void read_single_measurement(uint8_t reg){
    uint8_t measurement[2] = {0,0};
    twc_start((TWCStruct){0b11101100,Start,Transmit, &reg,1,0});
    twc_start((TWCStruct){0b11101101,Start,Recive,measurement,2,0});
    while (twc_status());
    char result[6];
    sprintf(result, "%u\n\r", measurement[1]);
    serialWrite(result);
}

void new_measurement(){
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

    dig_T1 = get_register(0x88);
    printer(dig_T1);
    dig_T2 = get_register(0x8A);
    printer(dig_T2);
    dig_T3 = get_register(0x8C);
    printer(dig_T3);

    while(1){
	new_measurement();
	_delay_ms(1000);
	//serialWrite("P");
	//read_measurement(0xF7,0);
	serialWrite("T");
	read_measurement(0xFA,1);
    }

}

