#include "i2c.h"
#include "serial.h"

void end_transmission(){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

uint8_t transmit(){
    //Wait for transmission to finish
    while(!(TWCR & (1<<TWINT)))
	;
    //Check the status register if transmission was successfull
    if (!(((TWSR & 0xF8) == 0x08) ||
	((TWSR & 0xF8) == 0x18) ||
	((TWSR & 0xF8) == 0x28) ||
	((TWSR & 0xF8) == 0x10) ||
	((TWSR & 0xF8) == 0x40) ||
	((TWSR & 0xF8) == 0x50) ||
	((TWSR & 0xF8) == 0x58)
	)){
	serialWrite("E");
	return (TWSR);
    }
    return 0;
}

uint8_t start_transmission(){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    return transmit();
}

void initTWC(){
    TWBR = 0b10000000;
    TWSR |= 0b00000000;

    // Enable internal pull-ups
    DDRC  &= ~((1<<PC5) | (1<<PC4));
    PORTC |=   (1<<PC5) | (1<<PC4);
}

uint8_t TWC_MT(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len ){
    uint8_t result = start_transmission();
    if (result != 0) {return result;}
    //Set slave address (this may include a RW bit) and start address transmission

    TWDR = _slave_addr;
    TWCR = (1<<TWINT) | (1<<TWEN);
    result = transmit();
    if (result != 0) {return result;}
    for (int i = 0; i < _data_len; i++){

	//Set send register to transmission data and begin transmission
	TWDR = _data[i];
	TWCR = (1<<TWINT) | (1<<TWEN);

	result = transmit();
	if (result != 0) {return result;}
    }
    end_transmission();
    return 0;
}

uint8_t TWC_MR(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len ){

    uint8_t result = start_transmission();
    if (result != 0) {return result;}

    //Set slave address (this may include a RW bit) and start address transmission
    TWDR = _slave_addr;
    TWCR = (1<<TWINT) | (1<<TWEN);
    result = transmit();
    if (result != 0) {return result;}

    for (int i = 0; i < _data_len; i++){
	//wait for transmission to finish
	while (!(TWCR & (1<<TWINT)));

	_data[i] = TWDR;

	//Send ACK or NACK
	if (i < _data_len - 1){
	    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);

	    result = transmit();
	    if (result != 0) {return result;}
	} else {
	    TWCR = (1<<TWINT) | (1<<TWEN);

	    result = transmit();
	    if (result != 0) {return result;}
	}

    }
    end_transmission();
    return 0;
}
