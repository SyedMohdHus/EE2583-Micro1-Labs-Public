;Lab 3 - Blink with interrupts (pic-as, PIC16F1829)
;Blue LED (RA2) blinks from the main loop. Green LED (RA5) blinks from the ISR.
;VERSION A: Challenge 1, Green LED toggled by the TMR0 overflow interrupt.
;Try Delay1, Delay2, Delay3 for the Blue LED in the main loop.
;Project Properties / pic-as Global Options / Additional options:
;            -Wl,-pMyCode=0h   -Wl,--fixupoverflow=ignore

; CONFIG1
  CONFIG  FOSC = INTOSC         ; INTOSC oscillator: I/O function on CLKIN pin
  CONFIG  WDTE = OFF            ; Watchdog Timer disabled
  CONFIG  PWRTE = OFF           ; Power-up Timer disabled
  CONFIG  MCLRE = ON            ; MCLR/VPP pin function is MCLR
  CONFIG  CP = OFF              ; Code protection disabled
  CONFIG  CPD = OFF             ; Data memory code protection disabled
  CONFIG  BOREN = ON            ; Brown-out Reset enabled
  CONFIG  CLKOUTEN = OFF        ; Clock Out disabled
  CONFIG  IESO = OFF            ; Internal/External Switchover disabled
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor disabled

; CONFIG2
  CONFIG  WRT = OFF             ; Write protection off
  CONFIG  PLLEN = OFF           ; PLL disabled
  CONFIG  STVREN = ON           ; Stack over/underflow causes Reset
  CONFIG  BORV = LO             ; Brown-out Reset voltage low
  CONFIG  DEBUG = OFF           ; leave OFF, tools manage it
  CONFIG  LVP = ON              ; Low-voltage programming enabled
; config statements should precede project file includes.

#include <xc.inc>

Mask1	EQU 1<<5      ; 0010 0000b, bit 5 (RA5 green, RB5 to Uno)

VarA	EQU 0X30
VarB	EQU 0X31
VarC	EQU 0X32

  PSECT MyCode,class=CODE,delta=2
  PAGESEL  (Start)
  GOTO     Start
  NOP                       ; The 3 NOPs are required, do not delete
  NOP
  NOP
;---------------- Interrupt vector is at 0004h ----------------
;ISR for TMR0 overflow (Green LED toggles every TMR0 rollover, about 65 ms)
  BANKSEL INTCON
  btfss   INTCON,2          ; was it TMR0IF?
  RESET                     ; not TMR0, restart
  bcf     INTCON,2          ; clear flag so we don't re-interrupt
  BANKSEL LATA
  movlw   Mask1
  xorwf   LATA,f            ; toggle Green LED (RA5)
  RETFIE                    ; return from interrupt (re-enables GIE)

Start:
	BANKSEL OSCCON
	movlw	0x6A            ; 4 MHz, Fosc/4 = 1 MHz
	movwf	OSCCON
	BANKSEL TRISA
	clrf	TRISA           ; PORTA all outputs
	BSF	TRISA,4         ; RA4 input (Arduino pulse)
	BANKSEL LATA
	clrf	LATA
	BSF	LATA,2          ; Blue LED off (common anode, 1 = off)
	BSF	LATA,5          ; Green LED off
	BANKSEL TRISB
	bsf	TRISB,7         ; push button RB7 input
	bcf	TRISB,5         ; RB5 output, goes to Uno D3 (Challenge 3)
	BANKSEL ANSELC
	clrf	ANSELC          ; all pins digital
	clrf	ANSELA
	clrf	ANSELB
	BANKSEL OPTION_REG
	MOVLW	0X87            ; Fosc/4, prescaler 1:256 on TMR0, weak pull-ups off
	MOVWF	OPTION_REG
;Interrupt setup: TMR0 overflow interrupt
	BANKSEL INTCON
	bcf	INTCON,2        ; clear TMR0 flag so it won't fire immediately
	bsf	INTCON,7        ; GIE on
	bsf	INTCON,5        ; TMR0IE on

;Start of Loop (Blue LED, RA2)
Loop:
	BANKSEL PORTB
	btfss	PORTB,7         ; button held -> stop blinking
	bra	Loop
	BANKSEL LATA
	BCF	LATA,2          ; Blue ON
	PAGESEL (Delay1)        ; <-- change number in 4 places: Delay1, Delay2 or Delay3
	CALL	Delay1
	BANKSEL LATA
	BSF	LATA,2          ; Blue OFF
	PAGESEL (Delay1)
	CALL	Delay1
	BRA	Loop
	;End of main routine

;Delay1: counting loop, about 0.2 s
Delay1:
    BANKSEL VarA
    movlw  0xFF
    movwf  VarA
Outside:
    decfsz VarA,f
    bra	Goon
    return
Goon:
    movlw 0xFF
    movwf VarB
Inside:
    decfsz VarB,f
    bra	Inside
    bra Outside

;Delay2: TMR0 polling delay, about 65 ms
Delay2:
    BANKSEL INTCON
    BCF INTCON,2            ; make sure flag is cleared at start
    BANKSEL TMR0
    MOVLW 0X01
    MOVWF TMR0
    BANKSEL INTCON
Tloop:
    BTFSC INTCON,2          ; wait till timer overflows
    RETURN
    BRA Tloop

;Delay3: wait for the Arduino pulse on RA4
Delay3:
	BANKSEL	PORTA
	BTFSS	PORTA,4
	BRA	Delay3
	BANKSEL	VarC
	CLRF	VarC
Wait5:	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	DECFSZ	VarC,f          ; small delay so the pulse is over
	BRA	Wait5
	RETURN
;End of Subroutines
 END


