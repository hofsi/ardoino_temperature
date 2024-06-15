#include "i2c.h"
#include "serial.h"
#include <stdint.h>


TWCStruct twcstruct;

uint8_t twc_status(){
    if (twcstruct.stage == Off)
	return 0;
    return 1;
}

void twc_init(){
    TWBR = 0b10000000;
    TWSR |= 0b00000000;

    /* Enable internal pull-ups */
    DDRC  &= ~((1<<PC5) | (1<<PC4));
    PORTC |=   (1<<PC5) | (1<<PC4);
    twcstruct.stage = Off;
    sei();
}

void twc_start(TWCStruct twcs){
    while(twc_status());
    twcstruct = twcs;
    sei();
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);
}

void twc_tr(){
    uint8_t twcr_carry = 0;
    twcstruct.stage = TR;
    if (twcstruct.mode == Transmit){
	TWDR = twcstruct.data[twcstruct.position];
    } else {
	twcstruct.data[twcstruct.position] = TWDR;
	twcr_carry |= (1<<TWEA);
    }
    twcstruct.position++;
    if (twcstruct.position >= twcstruct.length){
	twcstruct.stage = Stop;
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
    } else {
	TWCR |= twcr_carry | (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
    }
}

void twc_next(){
    switch (twcstruct.stage) {
	case Start:
	    TWDR = twcstruct.address;
	    twcstruct.stage = SlaveAddress;
	    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
	    break;
	case SlaveAddress:
	case TR:
	    twc_tr();
	    break;
	case Stop:
	    twcstruct.stage = Off;
	    TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE);
	    break;
	default:
	    TWCR = 0;
	    return;
    }
}

ISR(TWI_vect){
    /*Check for error*/
    if (!(((TWSR & 0xF8) == 0x08) ||
	((TWSR & 0xF8) == 0x18) ||
	((TWSR & 0xF8) == 0x28) ||
	((TWSR & 0xF8) == 0x10) ||
	((TWSR & 0xF8) == 0x40) ||
	((TWSR & 0xF8) == 0x50) ||
	((TWSR & 0xF8) == 0x58)
	)){
	serialWrite("ETWC2");
	char result[6];
	sprintf(result,"%i\n\r", TWSR);
	serialWrite(result);
	return;
    }

    /*Begin Transmission*/
   twc_next();
}
//=============OLD=CODE=========================

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

    sei();
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
