#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>


uint8_t initTWC_MT(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len );
uint8_t initTWC_MR(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len );

