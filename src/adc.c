#include "adc.h"

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
    char temp[sizeof(char) * 12]; 
    sprintf(temp , "%i\n\r", ADC);
    serialWrite(temp);
}

