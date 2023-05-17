#include <msp430.h> 
#define MAX 2621
#define MIN 524

/**
 * main.c
 */
void degrees(int type){
    switch(type){
    case 0:
        TA2CCR2=524;
    case 90:
        TA2CCR2=1049;
    case 180:
        TA2CCR2=2621;
    }
}
void io_config(){
    P2OUT &= ~BIT5;
    P2DIR |= BIT5;
    P2SEL |= BIT5;
}

void ta2_config(){
    TA2CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA2CCTL2 = OUTMOD_6;
    TA2CCR0 = 20971;
    TA2CCR2 = 1049;
}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	io_config();
	ta2_config();
	int carga = MIN;
	int sent = 0;
	while(1){
	    __delay_cycles(1000);
	    TA2CCR2 = carga;
	    if (sent == 0){
	        carga++;
	        if (carga == MAX){
	            sent = 1;
	        }
	    } else {
	        carga--;
	        if(carga==MIN){
	            sent=0;
	        }
	    }
	}
	return 0;
}
