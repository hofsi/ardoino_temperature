#include "i2c.h"
#include "serial.h"

void end_transmission(){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

uint8_t transmit(){
    //Wait for transmission to finish
    while(!(TWCR & (1<<TWINT)))
    //  ;
    {
	char buffer[3]; // Buffer to hold the ASCII representation of the value
	sprintf(buffer , "%i\n\r", TWCR);
	serialWrite(buffer);
	_delay_ms(1000);
    }
    //

    //Check the status register if transmission was successfull
    if ((TWSR & 0xF8) != 0x08){
	end_transmission();
	return TWSR;
    }
    return 0;
}

void start_transmission(){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 
    transmit();
}

void initTWC(){
    TWBR = 0b00000001;
}

uint8_t TWC_MT(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len ){
    
    //
    serialWrite("test\n\r"); 
    //
    
    start_transmission();
    
    //
    serialWrite("transmission started\n\r"); 
    //

    uint8_t result = transmit();
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
      
    start_transmission();
    
    uint8_t result = transmit();
    if (result != 0) {return result;}

    //Set slave address (this may include a RW bit) and start address transmission
    TWDR = _slave_addr;
    TWCR = (1<<TWINT) | (1<<TWEN);
    
    result = transmit();
    if (result != 0) {return result;}
   
    
    for (int i = 0; i < _data_len; i++){
	//wait for transmission to finish
	while (!(TWCR & (1<<TWINT)))
	    ;

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
