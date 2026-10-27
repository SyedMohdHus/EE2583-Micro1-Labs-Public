/*
 * File:   Lab_6.c
 * Starter code from the course, completed for the lab
 *
 * Capacitive touch on the 4 pads:
 *   U = RA4 = CPS3  -> Green
 *   T = RC7 = CPS9  -> Blue
 *   S = RC1 = CPS5  -> Red
 *   A = RC0 = CPS4  -> White
 *
 * Needs a CP2102 dongle on the PIC 6-pin UART header and PuTTY at 9600 8N1.
 *
 * Threshold:
 *   THRESHOLD = 0  -> auto mode. At power up (DON'T touch the pads) it reads each pad
 *                     16 times, and sets that pad's threshold to 85% of its untouched count.
 *                     The numbers are printed in PuTTY so you can write them on the sheet.
 *   THRESHOLD = 0x____ -> one fixed threshold for all pads, like the handout.
 *                     Pick it between the untouched and touched Tcount you see in PuTTY.
 *
 * MULTI_TOUCH = 1  -> challenge step 4: remembers every pad touched in a scan
 *                     and lights all of those colors at once, instead of only the last one.
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select  MCLR
#pragma config CP = OFF         // Flash Program Memory Protect disabled
#pragma config CPD = OFF        // Data Memory Code Protection disabled
#pragma config BOREN = OFF      // Brown-out Reset disabled
#pragma config CLKOUTEN = OFF   // Clock Out disabled.
#pragma config IESO = OFF       // Internal/External Switchover disabled
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable disabled

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection off
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable
#pragma config BORV = LO        // Brown-out Reset Voltage Selection low
#pragma config LVP = ON         // Low-Voltage Programming Enabled

/*Serial Configuration*/
#define BAUD 9600   //Bits per second transfer rate
#define FOSC 4000000L   //Frequency Oscillator
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))  //Should be 25 for 9600/4MhZ
#define NINE_BITS 0
#define SPEED 0x4       //T Speed
#define RX_PIN TRISC5   //Recieve Pin
#define TX_PIN TRISC4   //Transmit Pin

/*Xtal Macro*/
#define _XTAL_FREQ 4000000.0    /*for 4mhz*/
#define ScanNum  4      //Was 1 to find the threshold on U, now all 4 pads
#define THRESHOLD 0     //0 = auto calibrate each pad at power up, or put your fixed value here
#define MULTI_TOUCH 0   //1 = challenge step 4, show every pad touched

//Function Prototypes
void setup_comms(void);
void putch(char);
int getch(void);
char getche(void);
void timer_config(void);
void clockAndpin_config(void);
void usartConfig(void);
void Display (int);
void DisplayMulti(unsigned char *);
unsigned int ReadPad(int);

int main(int argc, char** argv) {
/*Variable Declaration*/
    unsigned int Tcount, Threshold[4];
    unsigned long Sum;
    int Touch[4], mode;
    unsigned char Touched[4];

/*TouchPad Setup*/
    CPSCON0 =   0x8C;   //Set up Touch sensing module control reg 0 (on, high range)
    CPSCON1 = 0x03;
    Touch[0] = 3; // RA4 pad CPS3  U
    Touch[1] = 9; // RC7 pad CPS9  T
    Touch[2] = 5; // RC1 pad CPS5  S
    Touch[3] = 4; // RC0 pad CPS4  A

/*Clock and Pin Configuration*/
    clockAndpin_config();      //Configures clock and pins, enables timers

/*USART CONFIG*/
    usartConfig();
    setup_comms();

/*Threshold Value*/
    printf("\n\rLab 6 touch\n\r");
    for (int j = 0; j < 4; j++) {
#if THRESHOLD == 0
        Sum = 0;
        for (int k = 0; k < 16; k++) Sum += ReadPad(Touch[j]);
        Threshold[j] = (unsigned int)((Sum / 16) * 85 / 100);   // 85% of untouched count
#else
        Threshold[j] = THRESHOLD;   //using one threshold for all 4 channels
#endif
        printf("Pad %d (CPS%d) Threshold=%X\n\r", j, Touch[j], Threshold[j]);
    }

/*Infinite While*/
    while (1){
        mode = 4; // 4 = no touch
        for (int j = 0; j < ScanNum; j++) {  //Change the ScanNum #define
           Tcount = ReadPad(Touch[j]);
           printf ("Tcount=%X, Threshold= %X \n\r", Tcount, Threshold[j]);
           Touched[j] = (Tcount < Threshold[j]);
           if (Touched[j]) mode = j;
        }
#if MULTI_TOUCH
        DisplayMulti(Touched);
#else
        Display(mode);
#endif
    }
 return (EXIT_SUCCESS);
}

// Select a pad, let Timer 0 set the fixed time window, return the Timer 1 count
unsigned int ReadPad(int channel) {
    unsigned int count;
    CPSCON1 = channel;
    timer_config();
    while (!TMR0IF) continue; //wait here till Timer 0 overflows
    count = (TMR1H << 8) + TMR1L; //Save 16 bit count value in timer 1
    TMR0IF = 0; //Clear the interrupt flag for Timer 0
    return count;
}

void Display(int delay) {
 switch (delay) {
 case 0: //Case for button U = Green
     LATA &= ~(1<<5);
     __delay_ms(500);
     LATA |=  (1<<5);
     __delay_ms(500);
     break;
 case 1: //Case for button T = Blue
     LATA &= ~(1<<2);
     __delay_ms(500);
     LATA |=  (1<<2);
     __delay_ms(500);
     break;
 case 2: //Case for button S = Red
     LATC &= ~(1<<6);
     __delay_ms(500);
     LATC |=  (1<<6);
     __delay_ms(500);
     break;
 case 3: //Case for button A = White
     LATA &= ~((1<<5)|(1<<2));
     LATC &= ~(1<<6);
     __delay_ms(500);
     LATA |=  (1<<5)|(1<<2);
     LATC |=  (1<<6);
     __delay_ms(500);
     break;
 case 4: // Default, No Touch
     LATA |=  (1<<5)|(1<<2);  //Turn all off
     LATC |=  (1<<6);
     break;
 }
}

// Challenge step 4: every touched pad turns its color on at the same time
// (A = white turns everything on). e.g. U + S together = green + red = yellow
void DisplayMulti(unsigned char *t) {
    unsigned char any = t[0] | t[1] | t[2] | t[3];
    if (t[0] || t[3]) LATA &= ~(1<<5);   // green
    if (t[1] || t[3]) LATA &= ~(1<<2);   // blue
    if (t[2] || t[3]) LATC &= ~(1<<6);   // red
    if (any) {
        __delay_ms(500);
        LATA |= (1<<5)|(1<<2);
        LATC |= (1<<6);
        __delay_ms(500);
    }
}


void clockAndpin_config(){
    OSCCON      =   0X6A;   //set up 4MHz for Fosc
    INTCON      =   0;      // purpose of disabling the interrupts.
    OPTION_REG  =   0XC5;   // Timer 0 on Fosc/4, 1:64 prescale = fixed time window
    T1CON       =   0XC1;   // Timer 1 clocked by the cap sense oscillator, on
    T1GCON      =   0X81;   // gate enabled, gate source = Timer 0 overflow (A1 is toggle mode)
    TRISA       =   0X10;   // RA5 Green & RA2 Blue LEDs are outputs, RA4=input (pad U)
    TRISC       =   0XBF;   // All port C= Inputs except RC6 = Red LED
    PORTA       =   0;
    LATA        =   0x24;   // green and blue off
    LATC        =   0x40;   // red off
    ANSELA      =   0X10;   // RA4 analog for the touch pad
    ANSELB      =   0X70;   // PortB analog x RB7 use RB5 for A2D for POT
    ANSELC      =   0X83;   // RC7, RC1, RC0 analog for the touch pads, RC4 & 5 digital for UART
}

void timer_config(){
    TMR1ON      =   0;
    TMR0        =   0;
    TMR1H       =   0;
    TMR1L       =   0;
    TMR1ON      =   1;
    TMR0IF      =   0;          //Clear the interrupt flag for Timer 0
    TMR0        =   0;
}

void usartConfig(){
    APFCON0 = 0x84; //This sets pins RC5 and RC4 as RX & TX on pic16f1829
    TXCKSEL = 1;
    RXDTSEL = 1;  //makes RC4 & 5 TX & RX for USART (Allows ICSP)
}

void setup_comms(void){

    RX_PIN = 1;
    TX_PIN = 1;
    SPBRG = DIVIDER;
    RCSTA = (NINE_BITS|0x90);
    TXSTA = (SPEED|NINE_BITS|0x20);
    TXEN=1;
    SYNC=0;
    SPEN=1;
    BRGH=1;
}

void putch(char byte)
{
    /* output one byte */
    while(!TXIF)    /* set when register is empty */
        continue;
    TXREG = byte;
}

int getch(void) {
    /* retrieve one byte */
    while(!RCIF)    /* set when register is not empty */
        continue;
    return RCREG;
}

char getche(void)
{
    char c;
    putch(c = getch());
    return c;
}
