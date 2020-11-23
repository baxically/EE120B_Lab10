/*    Author: Ally  Thach, athac007
 *  Partner(s) Name:
 *    Lab Section:
 *    Assignment: Lab #10  Exercise #1
 *    Exercise Description: [optional - include for your own benefit]
 *
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 *    DEMO LNIK:
 */
#include <avr/io.h>
#include "keypad.h"
#include "bit.h"
#include "scheduler.h"
#include "timer.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char tmpB = 0x00;
unsigned char x = 0;

enum kp_states {kp_keypad} ;
int keypadSMTick(int kp_currState)
{
    x = GetKeypadKey();

    switch(kp_currState)
    {
        case kp_keypad:
            kp_currState = kp_keypad;

            switch (x)
            {
                case '\0':
                    tmpB = 0x80;
                    break;
                case '1':
                    tmpB = 0x80;
                    break;
                case '2':
                    tmpB = 0x80;
                    break;
                case '3':
                    tmpB = 0x80;
                    break;
                case '4':
                    tmpB = 0x80;
                    break;
                case '5':
                    tmpB = 0x80;
                    break;
                case '6':
                    tmpB = 0x80;
                    break;
                case '7':
                    tmpB = 0x80;
                    break;
                case '8':
                    tmpB = 0x80;
                    break;
                case '9':
                    tmpB = 0x80;
                    break;
                case 'A':
                    tmpB = 0x80;
                    break;
                case 'B':
                    tmpB = 0x80;
                    break;
                case 'C':
                    tmpB = 0x80;
                    break;
                case 'D':
                    tmpB = 0x80;
                    break;
                case '*':
                    tmpB = 0x80;
                    break;
                case '0':
                    tmpB = 0x80;
                    break;
                case '#':
                    tmpB = 0x80;
                    break;
                default:
                    tmpB = 0x00;
                    break;
        
            }
            break;

        default:
            kp_currState = kp_keypad;
            break;
        }
    return kp_currState;
}

enum display_states {dp_display};
int displaySMTick(int dp_currState)
{
    unsigned char output;

    switch (dp_currState)
    {
        case dp_display:
            dp_currState = dp_display;
            break;

        default:
            dp_currState = dp_display;
            break;

    }

    switch (dp_currState)
    {
        case dp_display:
            output = tmpB;
            break;
    }
    PORTB = output;
    return dp_currState;
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF;
    PORTB = 0x00;
    DDRC = 0xF0;
    PORTC = 0x0F;
    
    static task task1, task2;
    task *tasks[] = { &task1, &task2 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    task1.state = start;
    task1.period = 50;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadSMTick;

    task2.state = start;
    task2.period = 10;
    task2.elapsedTime = task2.period;
    task2.TickFct = &displaySMTick;

    unsigned short i;
    unsigned long GCD = tasks[0]->period;
    for (i = 1; i < numTasks; i++)
    {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    while(1)
    {
        for (i = 0; i < numTasks; i++)
        {
            if ( tasks[i]->elapsedTime == tasks[i]->period)
            {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}
 
