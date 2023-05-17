#include <msp430.h> 


/**
 * main.c
 */

void io_config(){
    P4OUT &= ~BIT7;
    P4DIR |= BIT7;

    P1OUT &= ~BIT0;
    P1DIR |= BIT0;
}

void led_logic(){
    P4OUT ^= BIT7;
}

void ta0_config(){
    TA0CTL = TASSEL__ACLK | MC__UP | TACLR;
    TA0CCR0 = 16383;
    TA0CCTL0 = CCIE;
}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	io_config();
	ta0_config();
	
	__enable_interrupt();
	while(1){
	    __delay_cycles(100000);
	    P1OUT ^= BIT0;
	}
	return 0;
}

#pragma vector=53
__interrupt void ta0ccr0(void){
    P4OUT ^= BIT7;
}
