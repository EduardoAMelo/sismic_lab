#include <msp430.h> 
volatile int x, y;
volatile float vx,vy;

/**
 * main.c
 */
void ADC_config(void);
void ta0_config(void);
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    ta0_config();
    ADC_config();
    while(1){
        while((ADC12IFG & ADC12IFG7)==0);
        x = ADC12MEM0 + ADC12MEM2 + ADC12MEM4 + ADC12MEM6;
        y = ADC12MEM1 + ADC12MEM3 + ADC12MEM5 + ADC12MEM7;
        x /= 4;
        y /= 4;
        vx = (3.3/4095) * x;
        vy = (3.3/4095) * y;
    }
    return 0;
}

void ADC_config(void){
    ADC12CTL0 &= ~ADC12ENC; //Desabilitar para configurar
    ADC12CTL0 = ADC12ON; //Ligar ADC

    ADC12CTL1 = ADC12CONSEQ_3 | //Modo sequência de canais
            ADC12SHS_1 | //Selecionar TA0.1
            ADC12CSTARTADD_0 | //Resultado em ADC12MEM0
            ADC12SSEL_3; //ADC12CLK = SMCLK

    ADC12CTL2 = ADC12RES_2; //ADC12RES=0, Modo 8 bIts
    ADC12MCTL0 = ADC12SREF_0 | ADC12INCH_1; //Config MEM1
    ADC12MCTL1 = ADC12SREF_0 | ADC12INCH_2; //MEM2 = última
    ADC12MCTL2 = ADC12SREF_0 | ADC12INCH_1; //Config MEM1
    ADC12MCTL3 = ADC12SREF_0 | ADC12INCH_2; //MEM2 = última
    ADC12MCTL4 = ADC12SREF_0 | ADC12INCH_1; //Config MEM1
    ADC12MCTL5 = ADC12SREF_0 | ADC12INCH_2; //MEM2 = última
    ADC12MCTL6 = ADC12SREF_0 | ADC12INCH_1; //Config MEM1
    ADC12MCTL7 = ADC12SREF_0 | ADC12INCH_2 | ADC12EOS; //MEM2 = última
    P6SEL |= BIT2|BIT1; // Desligar digital de P6.2,1
    ADC12CTL0 |= ADC12ENC; //Habilitar ADC12
}
void ta0_config(void){
    TA0CTL = TASSEL__ACLK | MC__UP;
    TA0CCR0 = 1024;
    TA0CCTL1 = OUTMOD_6;
    TA0CCR1 = TA0CCR0/2;
}
