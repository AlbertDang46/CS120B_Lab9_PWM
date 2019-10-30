/*	Author: Albert Dang adang018
 *  Partner(s) Name: Min-Hua Wu mwu057
 *	Lab Section: 022
 *	Assignment: Lab #9  Exercise #2
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

enum State { Start, Off_Hold, Off, On_Hold, On, Press_Hold } state;
double freqs[] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25 };  
double curr_freq;
unsigned char i;

void tick();

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    
    PWM_on();
    
    state = Start;
    curr_freq = 0;
    i = 0;

    while (1) {
        tick();    
    }
    return 1;
}

void tick() {
    switch(state) {
        case Start:
            state = Off;
            break;
        case Off:
            state = PA0 ? On_Hold : Off;
            break;
        case On_Hold:
            state = PA0 ? On_Hold : On;
            break;
        case On:
            if (PA0) {
                state = Off_Hold;
            } else if (!PA0 && PA1 && !PA2) {
				state = Press_Hold;
				if (i < 7) {
					i++;
				}
			} else if (!PA0 && !PA1 && PA2) {
				state = Press_Hold;
				if (i > 0) {
					i--;
				}
			} else {
				state = On;
			}
            break;
        case Off_Hold:
            state = PA0 ? Off_Hold : Off;
            break;
		case Press_Hold:
			if (PA0) {
                state = Off_Hold;
            } else if (!PA0 && PA1 && !PA2) {
				state = Press_Hold;
			} else if (!PA0 && !PA1 && PA2) {
				state = Press_Hold;
			} else {
				state = On;
			}
            break;
        default:
            state = Start;
            break;
    }

    switch(state) {
        case Start:
			curr_freq = 0;
			i = 0;
            break;
        case Off:
            curr_freq = 0;
            i = 0;
            break;
        case On_Hold:
            break;
        case On:           
            curr_freq = freqs[i];
            break;
        case Off_Hold:
            break;
		case Press_Hold:
			curr_freq = freqs[i];
			break;
        default:
            curr_freq = 0;
            i = 0;
            break;
    }
	
    set_PWM(curr_freq);           
}
