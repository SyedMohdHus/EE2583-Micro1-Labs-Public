;Lab 2 - Timer0 delay, push button (RB7), Arduino pulse (RA4), indirect addressing
;Built from the Lab 2 starter (pic-as, PIC16F1829)
;Project Properties / pic-as Global Options / Additional options:
;            -Wl,-pMyCode=0h   -Wl,--fixupoverflow=ignore
;
; PIC16F1829 Configuration Bit Settings

; CONFIG1
  CONFIG  FOSC = INTOSC         ; INTOSC oscillator: I/O function on CLKIN pin
  CONFIG  WDTE = OFF            ; Watchdog Timer disabled
  CONFIG  PWRTE = OFF           ; Power-up Timer disabled
  CONFIG  MCLRE = ON            ; MCLR/VPP pin function is MCLR
  CONFIG  CP = OFF              ; Code protection disabled
  CONFIG  CPD = OFF             ; Data memory code protection disabled
  CONFIG  BOREN = ON            ; Brown-out Reset enabled
  CONFIG  CLKOUTEN = OFF        ; CLKOUT function disabled
  CONFIG  IESO = OFF            ; Internal/External Switchover disabled
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor disabled

; CONFIG2
  CONFIG  WRT = OFF             ; Write protection off
  CONFIG  PLLEN = OFF           ; 4x PLL disabled
  CONFIG  STVREN = ON           ; Stack over/underflow causes Reset
  CONFIG  BORV = LO             ; Brown-out Reset voltage, low trip point
  CONFIG  DEBUG = OFF           ; In-Circuit Debugger disabled
  CONFIG  LVP = ON              ; Low-voltage programming enabled

// config statements should precede project file includes.
#include <xc.inc>

VarA	EQU 0X30
VarB	EQU 0X31
VarC	EQU 0X32

  PSECT MyCode,class=CODE,delta=2
  PAGESEL  Start            ;This directive sets page bits for start
  GOTO    Start
  NOP                       ;The 3 NOP instructions are required
  NOP                       ;Do not delete any NOPs
  NOP                       ;No interrupts will occur until GIE set
  ;Start interrupt code here which is at 0004h
  RETFIE

Start:
	BANKSEL OSCCON
	movlw   0x6A            ; 4 MHz internal oscillator, Fosc/4 = 1 MHz
	movwf   OSCCON

	BANKSEL TRISA
	clrf    TRISA           ; PORTA all outputs (LED on RA5 and RA2)
	BSF     TRISA,4         ; Challenge 2: RA4 back to INPUT (Arduino pulse)
	BANKSEL ANSELA
	BCF     ANSELA,4        ; RA4 digital, not analog

	BANKSEL TRISB
	BSF     TRISB,7         ; RB7 push button = input
	BANKSEL ANSELB
	BCF     ANSELB,7        ; RB7 digital (harmless if no ANSEL bit exists)

	BANKSEL OPTION_REG
	MOVLW   0X87            ; 1000 0111: weak pull-ups off, Fosc/4, prescaler on TMR0, 1:256
	MOVWF   OPTION_REG

	; Challenge 3: point FSR0 at PORTB for indirect addressing
	movlw   PORTB
	movwf   FSR0L
	clrf    FSR0H           ; PORTB is in bank 0

	BANKSEL LATA
	clrf    LATA
	BSF     LATA,2          ; blue OFF (common anode: 1 = off) so we get pure green

;Start of Loop
Loop:
	; ---- check #1 (top of loop): button held -> LED stays OFF ----
	;Challenge 1 (direct) version:
	;BANKSEL PORTB
	;BTFSS   PORTB,7
	;BRA     Loop
	;Challenge 3 (indirect) version, no BANKSEL needed:
	BTFSS   INDF0,7         ; skip next if button NOT pressed (RB7 = 1)
	BRA     Loop            ; pressed -> jump back to top

	BANKSEL LATA
	BCF     LATA,5          ; green ON
	PAGESEL Delay2          ; <-- change to Delay3 for Challenge 2
	CALL    Delay2          ; <-- change to Delay3 for Challenge 2

	; ---- check #2 (middle of loop): button held -> LED stays ON ----
	BTFSS   INDF0,7
	BRA     Loop

	BANKSEL LATA
	BSF     LATA,5          ; green OFF
	PAGESEL Delay2          ; <-- change to Delay3 for Challenge 2
	CALL    Delay2          ; <-- change to Delay3 for Challenge 2
	BRA     Loop
	;End of main routine

; ---------- Lab 1 software delay (kept for reference, ~0.2 s) ----------
Delay:
    BANKSEL VarA
    movlw  0xFF
    movwf  VarA
Outside:
    decfsz VarA, f
    bra	Goon
    return
Goon:
    movlw 0xFF
    movwf VarB
Inside:
    decfsz VarB, f
    bra	Inside
    bra Outside

; ---------- Delay2: Timer0 delay, about 65 ms ----------
Delay2:
    BANKSEL INTCON
    BCF     INTCON,2        ; make sure TMR0IF flag is cleared at start
    BANKSEL TMR0
    MOVLW   0X01
    MOVWF   TMR0            ; preload 1: counts 255 x 256 us = 65.3 ms
    BANKSEL INTCON
Tloop:
    BTFSS   INTCON,2        ; wait till timer overflows (flag = 1)
    BRA     Tloop
    RETURN

; ---------- Delay3: wait for the pulse from the ATmega on RA4 ----------
Delay3:
    BANKSEL PORTA
    BTFSS   PORTA,4         ; wait until RA4 goes high (pulse starts)
    BRA     Delay3
    BANKSEL VarC
    CLRF    VarC
Wait5:
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    DECFSZ  VarC,f          ; just to add a small delay (~2.3 ms) so the pulse is over
    BRA     Wait5
    RETURN

 END


