/*
 * File:   Lab_5_PWM.c
 * Starter code from the course, completed for the lab
 *
 * Pot on RB5/AN11 (analog mini board) sets the PWM duty cycle of the LED.
 * Holding the RB7 push button makes the LED blink (TRISA5 toggles every loop).
 * Uses all 10 bits of the A2D result for the duty cycle.
 *
 * CHALLENGE7 = 0 : green only (steps 1 to 6 of the challenge)
 * CHALLENGE7 = 1 : step 7, all three colors on PWM
 *                  green RA5 = CCP2, blue RA2 = CCP3, red RC6 = CCP4, all on Timer 2
 *                  green follows the pot, blue is the opposite of the pot,
 *                  red follows the pot (so you sweep through a color fade)
 *
 * Remember: common anode, so a bigger duty cycle = LED pin high longer = DIMMER.
 */

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

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#define _XTAL_FREQ 4000000.0    /*for 4mhz*/

#define CHALLENGE7 0            // set to 1 for the 3 color version


//Function Prototypes
unsigned int ADC_Read(void);
void ADC_Initialize(void);
void PWM_Duty(unsigned int);
void PWM_Initialize(void);
void clockAndpin_config(void);
void PWM3_Duty(unsigned int);
void PWM4_Duty(unsigned int);
void PWM_RGB_Initialize(void);


//Global Variables
long PWM_freq = 1000;
int TMR2PRESCALE = 4;


void main()
{
//Variable Declarations
  unsigned int Pot;
  int adc_value;

//Tristate and ANSELx Configuration is done in clockAndpin_config():
//  RA5 output, RC2 digital input, RC3 analog input, RB5/AN11 analog input, RB7 button input

  //Initialization Function Calls (Should be 3 function calls here to init)
  clockAndpin_config();
  ADC_Initialize();
  PWM_Initialize();
#if CHALLENGE7
  PWM_RGB_Initialize();         // adds CCP3 (blue) and CCP4 (red)
#endif

  do
  {
  //Button Press Check
    if (!RB7) {
        TRISA5 = ~TRISA5;       //Toggle using bitwise not = blink while held
#if CHALLENGE7
        TRISA2 = TRISA5;        //blink blue and red along with green
        TRISC6 = TRISA5;
#endif
    }

  //Remaining Function Calls
    adc_value = ADC_Read();
    Pot = (unsigned int)adc_value;
    PWM_Duty(Pot);              // green
#if CHALLENGE7
    PWM3_Duty(1023 - Pot);      // blue goes the other way
    PWM4_Duty(Pot);             // red
#endif
    __delay_ms(50);

  }while(1); //Infinite Loop

}

//****************End Main*****************************************

//Put Function Definitions after main to make listing cleaner
//This means Function Prototypes are required in the header


void clockAndpin_config(void){
  //Tristate and ANSELx Configuration  Remember ANSELB 5 must remain in analog mode =1
  // All PWM channels should be set to output mode in the TRIS registers RA5, RA2, RC6
  // Push button must be set for input TRISB7 =1  and TRISB5=1  must be set as an input also for the
  // Pot input on pin RB5 as an analog input AN11
    OSCCON = 0x68; //  4MHz
    ANSELA = 0;    // all PortA pins to digital I/O
    ANSELB = 0X20; // AN11/RB5 as analog the rest 7,6,4 as digital
    ANSELC = 0X08; // RC3/AN7 analog (temp sensor spot), rest digital

    TRISA = 0x00;      // RA5 (and RA2) outputs
    TRISB = 0xA0;      //Output except RB7 and 5
    TRISC = 0X3C;      // RC2 digital in, RC3 analog in, RC4/RC5 UART, RC6 red LED output

    LATA  = 0X24;      //Turn off both green and Blue LED at start with hi
    LATB  = 0x00;
    LATC  = 0x40;      //Red off
}



void PWM_Initialize() {
    APFCON1 = 0x01;  //Set PWM to alternative PIN RA5 (HELLO WORLD LED)
    PR2 = 0xFF;  //PR2 value needed for 1000Hz frequency
    CCP2CON = 0x0C; //Enable pwm mode pg.238
    CCPR2L = 0x00;  //Set initial dutycycle to 0
    CCPTMRS = 0x00; //Set TMR2 to work with PWM
    PIR1 &= 0xFF;  //Clear TMR 2 int flag
    T2CON = 0b00000101; // 1:1 postscaler 4:1 prescaler on timer 2 pg.203
}


void PWM_Duty(unsigned int Dval)
{
    unsigned int SVal;  //Holds the CCP2CON
// Use all 10 bits of ADC result, Right Justify The ADC result
    SVal = CCP2CON;
    SVal &= 0XCF;  //erase the DC2B 0 and 1
    SVal |= ((Dval & 0X03)<<4) ;  //Zero out what is not 1 in bits 4-5
    CCP2CON = SVal;
    CCPR2L = (Dval>>2);

    return;
}


// Step 7: CCP3 is on RA2 (blue), CCP4 is on RC6 (red). CCPTMRS = 0 already puts both on Timer 2.
void PWM_RGB_Initialize(void) {
    CCP3CON = 0x0C;     // PWM mode
    CCPR3L  = 0x00;
    CCP4CON = 0x0C;     // PWM mode
    CCPR4L  = 0x00;
    TRISA2  = 0;        // blue output
    TRISC6  = 0;        // red output
}

void PWM3_Duty(unsigned int Dval)
{
    unsigned int SVal;
    SVal = CCP3CON;
    SVal &= 0XCF;
    SVal |= ((Dval & 0X03)<<4);
    CCP3CON = SVal;
    CCPR3L = (Dval>>2);
}

void PWM4_Duty(unsigned int Dval)
{
    unsigned int SVal;
    SVal = CCP4CON;
    SVal &= 0XCF;
    SVal |= ((Dval & 0X03)<<4);
    CCP4CON = SVal;
    CCPR4L = (Dval>>2);
}


void ADC_Initialize()
{
  ADCON0 = 0b00101101; //ADC ON and RB5/AN11
  ADCON1 = 0b11000000; // Right justified, Fosc/16, Vdd/Vss references
}



unsigned int ADC_Read()
{
  ADCON0 = 0b00101101;//Read on Channel 11 RB5/AN11 for pot on analog board

  __delay_ms(2); //Acquisition time delay
  GO_nDONE = 1; //Initializes A/D Conversion
  while(GO_nDONE); //Wait for A/D Conversion to complete
  return ((ADRESH<<8)+ADRESL); //Returns 10 Bit Result
}
