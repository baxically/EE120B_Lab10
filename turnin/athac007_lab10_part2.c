/*    Author: Ally  Thach, athac007
 *  Partner(s) Name:
 *    Lab Section:
 *    Assignment: Lab #10  Exercise #2
 *    Exercise Description: A household has a digital combination deadbolt lock system on the doorway. The system has buttons on a keypad. Pressing and releasing '#', then pressing and releasing the sequence '1',’2’,’3’,’4’,’5’, should unlock the door by setting PB0 to 1. Anytime you press and release the ‘#’ key this indicates the user is starting at the beginning of the sequence. Since we don’t have a solenoid or motor for locking, we will use an LED to represent this state. Any other sequence fails to unlock. Pressing a button from inside the house (PB7) locks the door (PB0=0). Be sure to check that only one button is pressed at a time.
 *
 *    I acknowledge all content contained herein, excluding template or example
 *    code, is my own original work.
 *    DEMO LNIK: https://drive.google.com/file/d/1QLpaQQ08vu6fvxlY9h9pk_bdA7NSHc6V/view?usp=sharing
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "bit.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#endif

unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char keypad_output = 0x00;
unsigned char pause = 0;

enum pauseButtonSM_States {pauseButton_wait, pauseButton_press, pauseButton_release};

int pauseButtonSMTick(int sm_currState)
{
    unsigned char press = ~PINA & 0x01;

    switch(sm_currState) {
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
    switch(led0_currState) {
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
    switch(led0_currState)
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
    switch(LED1_currState)
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

unsigned char tempB = 0x00;
enum lock_states {lock_L, lock_hashP, lock_hashR, lock_oneP, lock_oneR,
                    lock_twoP, lock_twoR, lock_threeP, lock_threeR,
                    lock_fourP, lock_fourR, lock_U} lock_currState;

void lockSMTick()
{
    switch (lock_currState) {
        case lock_L:
            lock_currState = (keypad_output == 0x0F) ? lock_hashP : lock_L;
            break;
        case lock_hashP:
            if (keypad_output == 0x1F)
            {
                lock_currState = lock_hashR;
                
            }
            else if (keypad_output == 0x0F)
            {
                lock_currState = lock_hashP;
                
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_hashR:
            if (keypad_output == 0x01)
            {
                lock_currState = lock_oneP;
            }
            else if (keypad_output == 0x1F)
            {
                lock_currState = lock_hashR;
            }
            else
            {
                lock_currState = lock_L;
                
            }
            break;
        case lock_oneP:
            if (keypad_output == 0x1F)
            {
                lock_currState = lock_oneR;
                
            }
            else if (keypad_output == 0x01)
            {
                lock_currState = lock_oneP;
                
            }
            else
            {
                lock_currState = lock_L;
                
            }
            break;
        case lock_oneR:
            if (keypad_output == 0x02)
            {
                lock_currState = lock_twoP;
                
            }
            else if (keypad_output == 0x1F)
            {
                lock_currState = lock_oneR;
                
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_twoP:
            if (keypad_output == 0x1F)
            {
                lock_currState = lock_twoR;
                
            }
            else if (keypad_output == 0x02)
            {
                lock_currState = lock_twoP;
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_twoR:
            if (keypad_output == 0x03)
            {
                lock_currState = lock_threeP;
            }
            else if (keypad_output == 0x1F)
            {
                lock_currState = lock_twoR;
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_threeP:
            if (keypad_output == 0x1F)
            {
                lock_currState = lock_threeR;
            }
            else if (keypad_output == 0x03)
            {
                lock_currState = lock_threeP;
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_threeR:
            if (keypad_output == 0x04)
            {
                lock_currState = lock_fourP;
            }
            else if (keypad_output == 0x1F)
            {
                lock_currState = lock_threeR;
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_fourP:
            if (keypad_output == 0x1F)
            {
                lock_currState = lock_fourR;
            }
            else if (keypad_output == 0x04)
            {
                lock_currState = lock_fourP;
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_fourR:
            if (keypad_output == 0x05)
            {
                lock_currState = lock_U;
            }
            else if (keypad_output == 0x1F)
            {
                lock_currState = lock_fourR;
                
            }
            else
            {
                lock_currState = lock_L;
            }
            break;
        case lock_U:
            lock_currState = (tempB) ? lock_L : lock_U ;
            break;

        default:
            break;
    }

    switch (lock_currState)
    {
        case lock_L:
            PORTB &= 0x80;
            break;
        case lock_hashP:
            break;
        case lock_hashR:
            break;
        case lock_oneP:
            break;
        case lock_oneR:
            break;
        case lock_twoP:
            break;
        case lock_twoR:
            break;
        case lock_threeP:
            break;
        case lock_threeR:
            break;
        case lock_fourP:
            break;
        case lock_fourR:
            break;
        case lock_U:
            PORTB |= 0x01;
            break;

        default:
            break;
    }
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

void set_PWM(double frequency) {
    static double current_frequency;

    if (frequency != current_frequency) {
        if (!frequency) {
            TCCR3B &= 0x08;
        }
        else {
            TCCR3B |= 0x03;
        }
        if (frequency < 0.954){
            OCR3A = 0xFFFF;
        }
        else if (frequency > 31250) {
            OCR3A = 0x0000;
        }
        else {
            OCR3A = (short)(8000000 / (128 * frequency)) - 1;
        }

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

enum States {Start, Wait, Play, Pressed} state;

unsigned char tempA = 0x00;
unsigned char z = 0x00;
double tune [] = {261.63, 432.43, 345.32, 700.34, 219.4};

void doorbellSMTick() {
  switch(state) {
    case Start:
        state = Wait;
        break;
    case Wait:
          if (tempA == 0x80) {
              state = Play;
              z = 0x00;
          }
          else {
              state = Wait;
          }
        break;
    case Play:
          if (z < 4) {
              state = Play;
          }
          else {
              state = Pressed;
          }
          break;
    case Pressed:
        if (tempA == 0x80) {
            state = Pressed;
        }
        else {
            state = Wait;
        }
        break;
    default:
        state = Start;
        break;
  }

  switch(state) {
    case Start:
        break;
    case Wait:
        break;
    case Play:
        set_PWM(tune[z]);
        z++;
        break;
    case Pressed:
        set_PWM(0);
        break;
    default:
        break;
  }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00;
    PORTA = 0xFF;
    DDRB = 0x7F;
    PORTB = 0x80;
    DDRC = 0xF0;
    PORTC = 0x0F;
    
    static task task1, task2, task3, task4, task5, task6;
    task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6};
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
    task4.TickFct = &lockSMTick;

    task5.state = start;
    task5.period = 100;
    task5.elapsedTime = task5.period;
    task5.TickFct = &keypadSMTick;

    task6.state = Start;
    z = 0x00;
    task6.period = 200;
    task6.elapsedTime = task6.period;
    task6.TickFct = &doorbellSMTick;

    unsigned long GCD = tasks[0]->period;
    unsigned short i;
    for (i = 1; i < numTasks; i++)
    {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();
    PWM_on();

    while (1) {
        for (i = 0; i < numTasks; i++)
        {
            tempA = ~PINA;
            tempB = ~PINB & 0x80;
            if (tasks[i]->elapsedTime == tasks[i]->period)
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
