/*	Author: Albert Dang adang018
 *  Partner(s) Name: Min-Hua Wu mwu057
 *	Lab Section: 022
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define PA0 (~PINA & 0x01)
#define PA1 (~PINA & 0x02)
#define PA2 (~PINA & 0x04)

void set_PWM(double frequency) {
    static double current_frequency;

    if (frequency != current_frequency) {
        if (!frequency) { TCCR3B &= 0x08; }
        else { TCCR3B |= 0x03; }

        if (frequency < 0.954) { OCR3A = 0xFFFF; }
        else if (frequency > 31250) { OCR3A = 0x0000; }
        else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on() {
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

enum State { Start, Play } state;
double freqs[] = { 261.63, 293.66, 329.63 };  
double curr_freq;

void tick();

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    
    PWM_on();
    
    state = Start;
    curr_freq = 0;

    while (1) {
        tick();    
    }
    return 1;
}

void tick() {
    switch(state) {
        case Start:
            state = Play;
            break;
        case Play:
            state = Play;
            break;
        default:
            state = Start;
            break;
    }

    switch(state) {
        case Start:
            curr_freq = 0;
            break;
        case Play:
            if (PA0 && !PA1 && !PA2) {
                curr_freq = freqs[0];
            } else if (!PA0 && PA1 && !PA2) {
                curr_freq = freqs[1];
            } else if (!PA0 && !PA1 && PA2) {
                curr_freq = freqs[2];
            } else {
                curr_freq = 0;
            }
            break;
        default:
            curr_freq = 0;
            break;
    }
	
    set_PWM(curr_freq);           
}
