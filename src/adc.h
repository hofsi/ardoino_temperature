#include "serial.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

void adcInit();
void adcStartConversion(int _ADCn);
