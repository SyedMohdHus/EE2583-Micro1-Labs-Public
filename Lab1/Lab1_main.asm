; Microcomputer Systems I
; Lab 1 Starter Code
; This starter code contains all steps covered in Lab 1 except for the challenge.
; Try to complete the challenge. Have fun!
    
; CONFIG1
  CONFIG  FOSC = INTOSC         ; Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
  CONFIG  WDTE = OFF            ; Watchdog Timer Enable (WDT disabled)
  CONFIG  PWRTE = OFF           ; Power-up Timer Enable (PWRT disabled)
  CONFIG  MCLRE = ON            ; MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
  CONFIG  CP = OFF              ; Flash Program Memory Code Protection (Program memory code protection is disabled)
  CONFIG  CPD = OFF             ; Data Memory Code Protection (Data memory code protection is disabled)
  CONFIG  BOREN = ON            ; Brown-out Reset Enable (Brown-out Reset enabled)
  CONFIG  CLKOUTEN = OFF        ; Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
  CONFIG  IESO = OFF            ; Internal/External Switchover (Internal/External Switchover mode is disabled)
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

; CONFIG2
  CONFIG  WRT = OFF             ; Flash Memory Self-Write Protection (Write protection off)
  CONFIG  PLLEN = OFF           ; PLL Enable (4x PLL disabled)
  CONFIG  STVREN = ON           ; Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
  CONFIG  BORV = LO             ; Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
  CONFIG  DEBUG = OFF           ; In-Circuit Debugger Mode (In-Circuit Debugger disabled, ICSPCLK and ICSPDAT are general purpose I/O pins)
  CONFIG  LVP = ON              ; Low-Voltage Programming Enable (Low-voltage programming enabled)

// config statements should precede project file includes.
#include <xc.inc> 
  
VarA	EQU 0X30
VarB	EQU 0X31
VarC	EQU 0X32
 
  
  PSECT MyCode,class=CODE,delta=2
  PAGESEL  Start     ;This directive sets page bits for start
  GOTO    Start
  NOP		     ;The 3 NOP instructions are required
  NOP                ;Do not delete any NOPs
  NOP                ;No interrupts will occur until GIE set
  ;Start interrupt code here which is at 0004h  
  RETFIE     ;  We end an interrupt routine with this return from interrupt.  
             ; Put here as placeholder.
Start:  ;Note that a colon after a label is required

;Clock Setup
    BANKSEL OSCCON
    movlw 0x6A
    movwf OSCCON
;Port Setup
    BANKSEL TRISA
    clrf TRISA
    BANKSEL LATA
    clrf LATA
    BSF  LATA,2      ;Blue OFF (common anode, 1 = off) so only Green blinks

;Challenge
;Start of Loop
Loop:
    ;Your Code here!
    
    BANKSEL LATA
    BSF     LATA,5
    PAGESEL Delay
    CALL    Delay
    BANKSEL LATA
    BCF     LATA,5
    PAGESEL Delay
    CALL    Delay 
    
    
    ;End of main routine
goto Loop

;Delay Routine

Delay:
        BANKSEL VarA
        movlw 0xFF
        movwf VarA
Outside:
        decfsz VarA, f
        goto Goon
        return
Goon:
        movlw 0xFF
        movwf VarB
Inside:
        decfsz VarB, f
        goto Inside
        goto Outside


 END



