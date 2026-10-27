/*
 * File:   Lab_7.c
 * Starter code from the course, completed for the lab
 *
 * A2D readings sent to PuTTY (CP2102 on the PIC UART header, 9600 8N1):
 *   Part A: internal temp sensor, Fixed Voltage Reference, pot (RB5/AN11)
 *   Part B: also the photoresistor and the external temp sensor on the analog board
 * Set PART_B to 1 for part B.
 */
#define PART_B 0
// 'C' source line config statements  
// #pragma config statements should precede project file includes.  
 
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
#pragma config LVP = ON        // Low-Voltage Programming Enabled

#include <xc.h>  
#include <stdio.h>  
#include <stdlib.h>  

  
//These are the values to set baud rate for UART and configure some functions
#define BAUD 9600  
#define FOSC 4000000L  
#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1)) // Should be 25 for9600/4MhZ  
#define NINE_BITS 0  
#define SPEED 0x4  
#define RX_PIN TRISC5  
#define TX_PIN TRISC4  
#define _XTAL_FREQ 4000000.0 /*for 4mhz*/  
  
//*Function Prototypes*//  
void RA5Blink(void);  
void pinConfig(void);  
void setup_comms(void);  
void putch(char);  
int getch(void);  
int getche(void);  
  
//*Global Vars*//  
unsigned int Temp, FVval, Pot, PCF, Photo;  
double Ctemp, Tcal;  
  
void main(void) {  
  
 pinConfig();  
  
setup_comms(); // set up the USART - settings defined in usart.h  
// Set up for the Temp Sensor, Pot  and FVR for PART A
//Later will add the PCF9700 Temp Sensor and the Photoresistor for PART B
FVRCON = 0xAF;  //This turns on the FVR and makes it available for the A2D
 //Add the internal temp sensor settings here.
 FVRCONbits.TSEN  = 1;   // FVRCON bit 5: temperature indicator on (already set by the 0xAF above)
 FVRCONbits.TSRNG = 0;   // FVRCON bit 4: low range, Vout = VDD - 2Vt, which is what the Ctemp math below uses
 
 
// Get set up for A2D  
 ADCON1 = 0xC3; //Right justify, clock=Fosc/4 and Vss and FVR -/+ references  
  
 while (1)  {
    
    /*Pot Read  it is on Chanel AN11  Thus the value in the ADCON0 reg = 0x2D */   
       ADCON0 = 0x2D; // set up for the pot input channel AN11
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       Pot = (unsigned)((ADRESH<<8)+(ADRESL)); //Store 10 bits into Pot, 8 + 2

    /*Fixed Voltage Reference Read*/     
       ADCON0 = 0x7D; // set up for the FVR input channel = 11111  
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       FVval = ((ADRESH<<8)+(ADRESL)); //Store 10 bits into FVval, 8 + 2
    
    /*Internal Temperature Read*/
       ADCON0 = 0x75; // set up for the temp input channel = 11101  
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       Temp = ((ADRESH<<8)+(ADRESL)); //Store 10 bits into Temp, 8 + 2

#if PART_B
    /*Photo Resistor Read*/     
       ADCON0 = 0x1D; // channel 00111 = AN7 = RC3, photoresistor
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       Photo = (unsigned int)((ADRESH<<8)+(ADRESL)); //Store 10 bits into Photo
    
    /*PCF9700 Temperature Read*/
       ADCON0 = 0x19; // channel 00110 = AN6 = RC2, external temp sensor
       __delay_ms(10); //Allow cap to recharge  
       ADGO = 1; // initiate conversion on the selected channel  
       while(ADGO)continue;  
       PCF = (unsigned int)((ADRESH<<8)+(ADRESL)); //Store 10 bits into PCF 
#endif

         
    /*Write to Terminal + Temperature Math*/   
       Ctemp = ((0.659-((1.48)*(1-((float)Temp/1023))))/0.00132)-40;  
       printf("IntT %d FixV %d Pot %d\n\r", Temp, FVval, Pot);
#if PART_B
       printf("PCF %d Photo %d CalT %f \n\r", PCF, Photo, Ctemp);
#endif
  
       __delay_ms(1000);  
         
    /*Blink RA5 once every transmission packet*/     
       RA5Blink();  
   }  
 return;  
  }  

void setup_comms(void)  {  
RX_PIN = 1;  
TX_PIN = 1;  
SPBRG = DIVIDER;  
RCSTA = (NINE_BITS | 0x90);  
TXSTA = (SPEED | NINE_BITS | 0x20);  
TXEN = 1;  
SYNC = 0;  
SPEN = 1;  
BRGH = 1;  
  }  

void putch(char bVal)  
  {  
/* output one byte */  
while(!TXIF) /* set when register is empty */  
continue;  
TXREG = bVal;  
  }  

int getch()  
  {  
/* retrieve one byte */  
while(!RCIF) /* set when register is not empty */  
continue;  
return RCREG;  
  }  

int getche(void)  
  {  
unsigned char c;  
putch(c = getch());  
return c;  
  }  
  
void pinConfig(void)  {  
 OSCCON = 0x6A; /* b6..4 = 1101 = 4MHz */  
 TXCKSEL = 1; // both bits in APFCON0 MUST BE 1 for 1829 0 for 1825  
 RXDTSEL = 1; /* makes RC4 & 5 TX & RX for USART (Allows ICSP)*/  
  
 TRISA = 0x10;  
 ANSELA =0x10;  
 TRISC = 0x0C; /* set as output */  
 ANSELC =0x0C; /* all ADC pins to digital I/O */  
 TRISB = 0xF0;
 ANSELB =0xF0;
 INTCON = 0; // purpose of disabling the interrupts.  
 LATA = 0x24; // green and blue off (common anode, 1 = off)
 LATC6 = 1;   // red off
  }  
  
void RA5Blink(void)  {  
LATA5 ^= 0x01; // Toggles the LED to help with debugging  
__delay_ms(200);   
  }  
 