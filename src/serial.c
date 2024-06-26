#include "serial.h"

typedef struct SerialBufferTransmit{
    char buffer[TX_BUFFER_SIZE];
    uint8_t readIndex;
    uint8_t writeIndex;
} SerialBufferTransmit;

typedef struct SerialBufferRecive{
    char buffer[RX_BUFFER_SIZE];
    uint8_t readIndex;
    uint8_t writeIndex;
} SerialBufferRecive;

#define INIT_BUFFER {{0}, 0, 0}

SerialBufferTransmit serialReciveBuffer = INIT_BUFFER;
SerialBufferRecive serialTransmitBuffer = INIT_BUFFER;

/*  Inits Serial Comunication Device
 *  If buffer size is 0 the struct is not created and transmission or reciving is disabled
 * */
void serialInit(){
    //Set baud rate
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;
    //Reset Serial Comunication Device
    UCSR0B = 0;
    //Set to 8-bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    //Enable Transmitter, Enable Transmission Complete Interrupt
    UCSR0B |= (1 << TXEN0)  | (1 << TXCIE0);

    //Enable Reciver, Enable Recive Complete Interrupt
    UCSR0B |= (1 << RXEN0) | (1 << RXCIE0);
    //Enable Interrupts
    sei();
}


/*  Adds a char to the serial buffer
 * */
void serialAppend(char _c){
    serialTransmitBuffer.buffer[serialTransmitBuffer.writeIndex] = _c;
    serialTransmitBuffer.writeIndex++;
    if (serialTransmitBuffer.writeIndex >= TX_BUFFER_SIZE){
	serialTransmitBuffer.writeIndex = 0;
    }
}

/*  Interrupt Call for Completed Transmission
 *  Begins transmission of next char in serial buffer
 *  Stops transmission when at write pos
 * */
ISR(USART_TX_vect){
    if(serialTransmitBuffer.readIndex != serialTransmitBuffer.writeIndex){
	UDR0 = serialTransmitBuffer.buffer[serialTransmitBuffer.readIndex];
	serialTransmitBuffer.readIndex++;
	if (serialTransmitBuffer.readIndex >= TX_BUFFER_SIZE){
	    serialTransmitBuffer.readIndex = 0;
	}
    }
}

/*  Writes String to Buffer
 *  String needs to be terminated
 *  Restarts Transmission
 * */
void serialWrite(char c[]){
    for (uint8_t i = 0; i < strlen(c); i++){
	serialAppend(c[i]);
    }
    //USARRT Data Register Empty => Transmit 0 to restart Transmission
    if(UCSR0A & (1 << UDRE0)){
	UDR0 = 0;
    }
}



