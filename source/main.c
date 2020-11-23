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
// #include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "bit.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"

unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char keypad_output = 0x00;
unsigned char pause = 0;

enum pauseButtonSM_States {pauseButton_wait, pauseButton_press, pauseButton_release};

int pauseButtonSMTick(int sm_currState)
{
    unsigned char press = ~PINA & 0x01;

    switch(cm_currState) {
        case pauseButton_wait:
            sm_currState = press == 0x01 ? pauseButton_press : pauseButton_wait;
            break;
        case pauseButton_press:
            sm_currState = pauseButton_release;
            break;
        case pauseButton_release:
            sm_currState = press == 0x00 ? pauseButton_wait : pauseButton_press;
            break;
        default:
            sm_currState = pauseButton_wait; break;
    }
    switch(sm_currState)
    {
        case pauseButton_wait:
            break;
        case pauseButton_press:
            pause = (pause == 0) ? 1 : 0;
            break;
        case pauseButton_release:
            break;
    }
    return sm_currState;
}

enum toggleLED0_State {toggleLED0_wait, toggleLED0_blink};

int toggleLED0SMTick(int led0_currState) {
    switch(state) {
        case toggleLED0_wait:
            led0_currState = !pause ? toggleLED0_blink: toggleLED0_wait;
            break;
        case toggleLED0_blink:
            led0_currState = pause ? toggleLED0_wait: toggleLED0_blink;
            break;
        default:
            led0_currState = toggleLED0_wait;
            break;
    }
    switch(led0_currSstate)
    {
        case toggleLED0_wait:
            break;
        case toggleLED0_blink:
            led0_output = (led0_output == 0x00) ? 0x01 : 0x00;
            break;
    }
    return led0_currState;
}

enum toggleLED1_State {toggleLED1_wait, toggleLED1_blink};

int toggleLED1SMTick(int LED1_currState)
{
    switch(LED1_currState) {
        case toggleLED1_wait:
            LED1_currState = !pause ? toggleLED1_blink: toggleLED1_wait;
            break;
        case toggleLED1_blink:
            LED1_currState = pause ? toggleLED1_wait: toggleLED1_blink;
            break;
        default:
            LED1_currState = toggleLED1_wait;
            break;
    }
    switch(state)
    {
        case toggleLED1_wait:
            break;
        case toggleLED1_blink:
            led1_output = (led1_output == 0x00) ? 0x01: 0x00;
            break;
    }
    return LED1_currState;
}

enum display_States {display_display};

int displaySMTick(int dis_sm_currState)
{
    switch(dis_sm_currState)
    {
         case display_display:
            dis_sm_currState = display_display; break;
         default:
            dis_sm_currState = display_display; break;
     }
     switch (dis_sm_currState)
     {
         case display_display:
         if (keypad_output != 0x1F)
         {
             PORTB = 0x80;
         }
         else
         {
             PORTB = 0x00;
         }
         break;
     }
    return dis_sm_currState;
 }

enum keypad_States {keypad_keypad};

int keypadSMTick(int kp_currState) {
    unsigned char x;
    switch (kp_currState) {
        case keypad_keypad:
            x = GetKeypadKey();
            switch (x) {
                case '\0': keypad_output = 0x1F; break;
                case '1': keypad_output = 0x01; break;
                case '2': keypad_output = 0x02; break;
                case '3': keypad_output = 0x03; break;
                case '4': keypad_output = 0x04; break;
                case '5': keypad_output = 0x05; break;
                case '6': keypad_output = 0x06; break;
                case '7': keypad_output = 0x07; break;
                case '8': keypad_output = 0x08; break;
                case '9': keypad_output = 0x09; break;
                case 'A': keypad_output = 0x0A; break;
                case 'B': keypad_output = 0x0B; break;
                case 'C': keypad_output = 0x0C; break;
                case 'D': keypad_output = 0x0D; break;
                case '*': keypad_output = 0x0E; break;
                case '0': keypad_output = 0x00; break;
                case '#': keypad_output = 0x0F; break;
                default: keypad_output = 0x1B; break;
            }
            kp_currState = keypad_keypad;
            break;

        default:
            kp_currState = keypad_keypad;
            break;
    }

    switch (kp_currState) {
        default: break;
    }
    
    return kp_currState;
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00;
    PORTA = 0xFF;
    DDRB = 0xFF;
    PORTB = 0x00;
    DDRC = 0xF0;
    PORTC = 0x0F;
    
    static task task1, task2, task3, task4, task5;
    task *tasks[] = {&task1, &task2, &task3, &task4, &task5};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    
    task1.state = start;
    task1.period = 50;
    task1.elapsedTime = task1.period;
    task1.TickFct = &pauseButtonSMTick;
    
    task2.state = start;
    task2.period = 500;
    task2.elapsedTime = task2.period;
    task2.TickFct = &toggleLED0SMTick;
    
    task3.state = start;
    task3.period = 1000;
    task3.elapsedTime = task3.period;
    task3.TickFct = &toggleLED1SMTick;
    
    task4.state = start;
    task4.period = 10;
    task4.elapsedTime = task4.period;
    task4.TickFct = &displaySMTick;
    
    task5.state = start;
    task5.period = 100;
    task5.elapsedTime = task5.period;
    task5.TickFct = &keypadSMTick;

    unsigned long GCD = tasks[0]->period;
    unsigned short i;
    for (i = 1; i < numTasks; i++)
    {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    while (1) {
        for (i = 0; i < numTasks; i++)
        {
            if (tasks[i]->elapsedTime == tasks[i]->period)
            {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }

        while(!TimerFlag)
        {
            
        }
        TimerFlag = 0;
    }
    
    return 0;
}
