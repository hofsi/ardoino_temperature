#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>

#define CLK 16000000
#define BUAD 9600
#define BRC (CLK/16/BUAD - 1)



typedef struct SerialBuffer{
    char *buffer;
    uint8_t size;
    uint8_t readIndex;
    uint8_t writeIndex;
} SerialBuffer;

SerialBuffer *initSerialBuffer(uint8_t size) {
    SerialBuffer *serialBuffer = (SerialBuffer *)malloc(sizeof(SerialBuffer));
    serialBuffer->buffer = (char *)malloc(size);
    serialBuffer->size = size;
    serialBuffer->readIndex = 0;
    serialBuffer->writeIndex = 0;
    return serialBuffer;
}

SerialBuffer serialBuffer;

void appendSerial(char _c);
void serialWrite(char _c[]);
void serialInit();
void adcInit();
void adcStartConversion(int _ADCn);

int main(void){
    
    serialInit();
    adcInit();
    while(1){
	_delay_ms(500);
	adcStartConversion(0);
    }
}


void serialInit(int bufferSize){
    //Set baud rate
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;
    //Enable Transmitter, Enable Transmission Complete Interrupt
    UCSR0B = (1 << TXEN0)  | (1 << TXCIE0);
    //Set to 8-bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    //Enable Interrupts
    sei();
    
    serialBuffer = *initSerialBuffer(16);
}


/*  Adds a char to the serial buffer
 * */
void appendSerial(char _c){
    serialBuffer.buffer[serialBuffer.writeIndex] = _c;
    serialBuffer.writeIndex++;
    if (serialBuffer.writeIndex >= serialBuffer.size){
	serialBuffer.writeIndex = 0;
    }
}

/*  Interrupt Call for Completed Transmission
 *  Begins transmission of next char in serial buffer
 *  Stops transmission when at write pos
 * */
ISR(USART_TX_vect){
    if(serialBuffer.readIndex != serialBuffer.writeIndex){
	UDR0 = serialBuffer.buffer[serialBuffer.readIndex];
	serialBuffer.readIndex++;
	if (serialBuffer.readIndex >= serialBuffer.size){
	    serialBuffer.readIndex = 0;
	}
    }
}

/*  Writes String to Buffer
 *	String needs to be terminated
 *  Restarts Transmission
 * */
void serialWrite(char c[]){
    uint8_t i = 0;
    for (i; i < strlen(c); i++){
	appendSerial(c[i]);
    }
    //USARRT Data Register Empty => Transmit 0 to restart Transmission
    if(UCSR0A & (1 << UDRE0)){
	UDR0 = 0;
    }
}



void adcInit(){
    //Sets Reference Voltage to AVcc (+5V)
    ADMUX = (1 << REFS0); 
    //Enable ADC, Enable ADC Interrupt, sets clock prescaler to 128
    ADCSRA = (1 << ADEN) | (1 << ADIE) | 0b111 ; 
    sei();
}

void adcStartConversion(int _ADCn){
    //Selects ADCn pin for ADC
    ADMUX |= 0b1111;
    ADMUX ^= 0b1111;
    ADMUX |= _ADCn;
    //Disables Digital Input for ADCn
    DIDR0 |= (1 << _ADCn);
    //Starts Conversion
    ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect){
    char temp[sizeof(char) * 20]; 
    sprintf(temp , "%i\n\r", ADC);
    serialWrite(temp);
}

