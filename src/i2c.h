#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>

typedef enum TWCStage{
    Start, SlaveAddress, TR, Stop, Off
}TWCStage;

typedef enum TWCMode{
    Transmit, Recive,
} TWCMode;


typedef struct TWCStruct{
    uint8_t address;
    TWCStage stage;
    TWCMode mode;
    uint8_t *data;
    uint8_t length;
    uint8_t position;
} TWCStruct;

void twc_init();
void twc_start(TWCStruct twcs);

uint8_t TWC_MT(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len );
uint8_t TWC_MR(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len );
void initTWC();
