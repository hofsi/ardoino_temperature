

uint8_t transmit(){
    //Wait for transmission to finish
    while(!(TWCR&(1<<TWINT)))
	;

    //Check the status register if transmission was successfull
    if ((TWSR & 0xF8) != START)
	end_transmission();
	return TWSR;
}


void start_transmission(){
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)
    transmit();
}

void end_transmission(){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}


uint8_t initTWC_MT(uint8_t _slave_addr, uint8_t _data ){
    
    start_transmission();
    
    uint8_t result = transmit();
    if (result != 0) {return result;}

    //Set slave address (this may include a RW bit) and start address transmission
    TWDR = _slave_addr;
    TWCR = (1<<TWINT) | (1<<TWEN);
    
    uint8_t result = transmit();
    if (result != 0) {return result;}
    
    //Set send register to transmission data and begin transmission
    TWDR = _data;
    TWCR = (1<<TWINT) | (1<<TWEN);

    uint8_t result = transmit();
    if (result != 0) {return result;}

    end_transmission();
    return 0;
}

uint8_t initTWC_MR(uint8_t _slave_addr, uint8_t* _data, uint8_t _data_len ){
      
    start_transmission();
    
    uint8_t result = transmit();
    if (result != 0) {return result;}

    //Set slave address (this may include a RW bit) and start address transmission
    TWDR = _slave_addr;
    TWCR = (1<<TWINT) | (1<<TWEN);
    
    uint8_t result = transmit();
    if (result != 0) {return result;}
   
    
    for (int i = 0; i < _data_len; i++){
	//wait for transmission to finish
	while (!(TWCR & (1<<TWINT)))
	    ;

	_data[i] = TWDR;
	
	//Send ACK or NACK
	if i < _data_len - 1{
	    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);

	    uint8_t result = transmit();
	    if (result != 0) {return result;} 	
	} else {
	    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);

	    uint8_t result = transmit();
	    if (result != 0) {return result;} 
	}

    }
    end_transmission();
    return 0;
}
