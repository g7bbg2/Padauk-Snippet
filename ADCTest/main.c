/*
  Serial_HelloWorld

  Transmits "Hello World" over Serial @ ~115200 baud, and repeats once a second.
*/

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "serial.h"
#include "delay.h"

#define ADC_BIT 4
//#define	ADC_ASM	1

// Note: serial.h assumes TX is on the PA7 pin (Port A, Bit 7), and uses timer2 (TM2) interrupts for timing.
//uint8_t adcval;
void interrupt(void) __interrupt(0) {
  if (INTRQ & INTRQ_TM2) {        // TM2 interrupt request?
    INTRQ &= ~INTRQ_TM2;          // Mark TM2 interrupt request processed
    serial_irq_handler();         // Process next Serial Bit
  }
}
// use comparator as 4 bit A/D
// by changing the reference voltage ...
// ... and checking the comparator result bit
#ifdef	ADC_ASM
//in assembler 
uint8_t	adcval;
uint8_t	analogRead(void){
	__asm__(
"	mov a,#0x0f\n\t"
"	mov	_adcval,a\n\t"
"$0001:	mov a,_adcval\n\t"
"	mov __gpcs,a\n\t"
"	nop\n\t"
"	nop\n\t"
"	t0sn __gpcc,#6\n\t"
"	goto $0002\n\t"
"	dzsn _adcval\n\t"
"	goto $0001\n\t"
"$0002:\n\t");
	return(adcval);
}
#else
// .... or C
uint8_t	analogRead(void){
uint8_t	adcval;
	adcval=0x0f;
	while(adcval){
		GPCS=adcval;
		__asm__("nop\n");
		__asm__("nop\n");
		if(GPCC & 0x40) break;
		adcval--;
	}
	return(adcval);
}
#endif
// print argumet as 2 hex digits
void pHex(uint8_t ch){
uint8_t tmp;
	tmp=ch >> 4;
	(tmp >9)?putchar(tmp+'7'):putchar(tmp+'0');
	tmp =ch & 0x0f;
	(tmp >9)?putchar(tmp+'7'):putchar(tmp+'0');
}
// print a null terminated string to serial console
void pstr(char *S){
	while(*S) putchar(*S++);
}

// Main program	
void main() {

  // Initialize hardware
  serial_setup();                 // Initialize Serial engine
  PAC &=~(1 << ADC_BIT);
  PADIER |= ( 1 << ADC_BIT);       // Enable ADC pin as digital input
  GPCS=0b00000111;			// setup comparator
  GPCC=0b10000111;

  INTRQ = 0;
  __engint();                     // Enable global interrupts

  // Main processing loop
  while (1) {
    pstr("ADC value=");
    pHex(analogRead());
    serial_println("");
    _delay_ms(1000);
   }
}

// Startup code - Setup/calibrate system clock
unsigned char _sdcc_external_startup(void) {

  // Initialize the system clock (CLKMD register) with the IHRC, ILRC, or EOSC clock source and correct divider.
  // The AUTO_INIT_SYSCLOCK() macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC clock source and divider.
  // Alternatively, replace this with the more specific PDK_SET_SYSCLOCK(...) macro from pdk/sysclock.h
  AUTO_INIT_SYSCLOCK();

  // Insert placeholder code to tell EasyPdkProg to calibrate the IHRC or ILRC internal oscillator.
  // The AUTO_CALIBRATE_SYSCLOCK(...) macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC oscillator.
  // Alternatively, replace this with the more specific EASY_PDK_CALIBRATE_IHRC(...) or
  // EASY_PDK_CALIBRATE_ILRC(...) macro from easy-pdk\calibrate.h
  AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

  return 0;   // Return 0 to inform SDCC to continue with normal initialization.
}
