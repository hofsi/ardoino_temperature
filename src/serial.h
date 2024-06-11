#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CLK 16000000
#define BUAD 9600
#define BRC (CLK/16/BUAD - 1)
#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

void serialAppend(char _c);
void serialWrite(char _c[]);
void serialInit();

