;Lab 4 Starter Code
;Jul 30, 2024


; Assembly source line config statements for PIC16F1829
; CONFIG1
  CONFIG  FOSC = INTOSC  ; Oscillator Selection (INTOSC oscillator
  CONFIG  WDTE = OFF     ; Watchdog Timer Enable (WDT disabled)
  CONFIG  PWRTE = OFF    ; Power-up Timer Enable (PWRT disabled)
  CONFIG  MCLRE = ON     ; (MCLR/VPP pin function is MCLR)
  CONFIG  CP = OFF       ; Flash Program Memory Code Protection disabled)
  CONFIG  CPD = OFF      ; Data Memory Code Protection disabled
  CONFIG  BOREN = ON     ; Brown-out Reset Enable (Brown-out Reset enabled)
  CONFIG  CLKOUTEN = OFF ; Clock Out disabled
  CONFIG  IESO = OFF     ; Internal/External Switchover ( disabled)
  CONFIG  FCMEN = OFF    ; Fail-Safe Clock Monitor disabled

; CONFIG2
  CONFIG  WRT = OFF      ; Flash Memory Self-Write Protection off
  CONFIG  PLLEN = OFF    ; PLL Enable (4x PLL disabled)
  CONFIG  STVREN = ON    ; Stack Overflow or Underflow will cause a Reset
  CONFIG  BORV = LO      ; Brown-out Reset Voltage Selection ( low selected)
  CONFIG  DEBUG = OFF    ; In-Circuit Debugger Mode OFF,auto managed by tools
  CONFIG  LVP = ON       ; Low-Voltage Programming Enabled
; config statements should precede project file includes.
 
#include <xc.inc> 

;Put the EQU declarations here  
;Start names in column 1

VarA	EQU 0X20 ;first RAM location used for the delay
VarB	EQU 0X21
  
  PSECT MyCode,class=CODE,delta=2   ;This tells assembler that code follows
;Don't forget-paste in project properties  -Wl,-pMyCode=0h   -Wl,--fixupoverflow=ignore
  PAGESEL  (Start)            ;This directive sets page bits for start
  GOTO     Start
  NOP		          ;The 3 NOP instructions are required
  NOP                            ;Do not delete any NOPs
  NOP                           ;No interrupts will occur until GIE set
  ;Start interrupt code here, which is at 0004h  
  RETFIE       ;  We end an interrupt routine with this return from interrupt.  

Start:     ;Main setup code
    BANKSEL OSCCON             ;set the clock FIRST so ADC/PWM timing is right from the start
    MOVLW   0X6A               ;4 MHz internal oscillator
    MOVWF   OSCCON
    PAGESEL  (ADC_Initilize)   ;set page bits for ADC_Initilize
    Call ADC_Initilize
    PAGESEL  (PWM_Initilize)   ;set page bits for PWM_Initilize
    Call PWM_Initilize
    
Wait1:
    BANKSEL PIR1
    BTFSS   PIR1, 1
    BRA    Wait1
    BANKSEL TRISA
    BCF	    TRISA, 5   ;Starts the PWM working
    
Loop_Main:
    PAGESEL (ADC_Read)
    Call    ADC_Read           ;new 8 bit pot value in ADRESH
    PAGESEL (PWM_Duty)
    Call    PWM_Duty           ;write it into the PWM duty cycle register
    PAGESEL (Delay)
    Call    Delay              ;short delay, about 25 ms
    BRA    Loop_Main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ADC_Initilize:
	Banksel ADCON0	;Note  ADCON0 and ADCON1 in same bank
	MOVLW	0X2D    ;Set channel to AN11/RB5, turn on ADC 
	MOVWF	ADCON0
	MOVLW	0X40	;Left justified, Fosc/4 and Vss and Vdd as ref
	MOVWF	ADCON1
	BANKSEL	TRISB	;Configure RB5 as analog input
	BSF	TRISB,5
	BANKSEL	ANSELB
	BSF	ANSELB,5
	RETURN
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ADC_Read:
	BANKSEL ADCON0
	BSF	ADCON0,1  ;This starts a conversion process
ADC_Wait:
	BTFSC	ADCON0,1  ;GO/DONE clears when the conversion is done
	BRA	ADC_Wait
	RETURN			;Returns when done with result in ADRESH
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;	
PWM_Initilize:
	BANKSEL	APFCON1
	BSF	APFCON1, 0
	BANKSEL	TRISA
	BSF	TRISA,5	;This is a safety move, we will turn on later
	BANKSEL	PR2
	MOVLW	0XFF
	MOVWF	PR2		;This will give about 1KHz as a pulse frequency
	BANKSEL	CCP2CON	;Most of the CCP2 registers are in the same bank	
	MOVLW	0X0C
	MOVWF	CCP2CON
	BANKSEL	CCPR2L
	CLRF	CCPR2L		;Set initial duty cycle to 0
	BANKSEL	CCPTMRS
	CLRF	CCPTMRS	;Select timer 2 for all ccp modules
	BANKSEL	PIR1
	BCF	PIR1, 1		;clear TMR2IF (it lives in PIR1, Wait1 polls PIR1,1)
	BANKSEL	T2CON	
	MOVLW	00000101B  ;1:1 postscaler, 4:1 prescaler and TMR2 turned on
	MOVWF	T2CON
	RETURN 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
PWM_Duty:
	BANKSEL	ADRESH
	MOVF	ADRESH, W
	BANKSEL	CCPR2L
	MOVWF	CCPR2L
	RETURN
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Delay: 
      BANKSEL VarA 
      movlw 0x20     ;change this from 0XFF 
      movwf VarA 
Outside: 
      decfsz VarA,f 
      BRA Goon 
      RETURN
Goon: 
      movlw 0xFF 
      movwf VarB
Inside:
      decfsz VarB,f
      BRA Inside
      BRA Outside

    END


