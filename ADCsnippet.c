
// PA4 is used as input to Comparator
#define ADC_BIT 4


// initialise the IO as input
  PAC &=~(1 << ADC_BIT);
  PADIER |= ( 1 << ADC_BIT);       // Enable ADC pin as digital input
  GPCS=0b00000111;			// setup comparator
  GPCC=0b10000111;

// use comparator as a simple ADC return 0-15 
uint8_t	analogRead(void){
uint8_t	adcval;
	adcval=0x0f;		// start at maximum value (on -ve input=VDD) 
	while(adcval){
		GPCS=adcval;	// set the comparator value
		__asm__("nop\n");	// let it settle (may not be needed)
		__asm__("nop\n");	
		if(GPCC & 0x40) break;	// get out if +ve is > negative  
		adcval--;
	}
	return(adcval);
}
