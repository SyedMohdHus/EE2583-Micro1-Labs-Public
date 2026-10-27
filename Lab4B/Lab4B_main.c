/*
 * File:   Lab_4B.c   (first C lab, XC8 compiler)
 * Starter code from the course, completed for the lab
 *
 * Blinks the green LED on RA5 every 500 ms.
 * RGB LED is common anode, so writing a 0 turns a color ON.
 *
 * MPLAB X: New Project > Standalone > PIC16F1829 > Snap > XC8 (NOT pic-as).
 * No extra linker options needed for C projects.
 */

// PIC16F1829 Configuration Bit Settings

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config DEBUG = OFF      // In-Circuit Debugger Mode (In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

#include <xc.h>
// #include "UART.h"   // not needed for the blink, add it back only if your handout uses it

#define _XTAL_FREQ 4000000.0    // 4 MHz, needed by __delay_ms()

void main(void) {
    //Clock Setup
    OSCCON = 0x68;      // 0b0110 1000: IRCF = 1101 = 4 MHz internal clock
    //Tristate and Port Configs
    TRISA  = 0x00;      // all of port A outputs (RA5 green, RA2 blue)
    PORTA  = 0x00;
    TRISB  = 0x80;      // RB7 push button is an input, the rest outputs
    ANSELB = 0x00;      // port B all digital
    TRISC6 = 0;         // RC6 red LED is an output
    LATC6  = 1;         // red off
    // Turn off the green and blue LEDs here (1 = off, common anode)
    LATA   = 0xFF;
    //Start of the infinite while loop portion
    while(1){
        LATA &= ~(1<<5);    // 1. green ON  (RA5 = 0)
        __delay_ms(500);    // 2. wait
        LATA |= (1<<5);     // 3. green OFF (RA5 = 1)
        __delay_ms(500);    // 4. wait
    }
}
