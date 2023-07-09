#include <msp430.h>
#include <string.h>
#define TRUE 1


// Declaracao de variaveis globais
volatile long int x, y, dif;
volatile int gas_sensor;
int tempo[50];


// declaracao de funcoes
void USCI_A0_config(void);
void adc_config(void);
void ser_char(char x);
void ser_str(char *x);
void ser_dec8(char x);
void ser_dec16(char x);
void ser_clr(void);
void io_config(void);
void ta2_config(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    // chamando todas as configuracoes
    USCI_A0_config(); // bluetooth
    io_config();
    ta2_config();
    adc_config(); // MQ-135 (SENSOR DE GAS)

    while (TRUE)
    {
        // ATIVANDO A CONVERSAO
        ADC12CTL0 |= ADC12SC;
        ADC12CTL0 &= ~ADC12SC;

        // ESPERANDO A CONVERSAO FINALIZAR
        while ((ADC12IFG & BIT0) == 0);
        gas_sensor = ADC12MEM0;
        ser_str("CO2: ");
        ser_dec16(gas_sensor);
        ser_str("ppmm");
        ser_char('\n');
        __delay_cycles(100000); // apenas para ficar mais suave no display serial
        ser_clr(); // serve para ficar sempre na mesma linha

        // TODO (CODIGO DO SENSOR DE UMIDADE E TEMPERATURA)
        //     volatile long int x, y, dif;

        //     int tempo[50];

        //     for (int i = 0; i < 50; i++)
        //     {
        //         TA2CCTL2 = CM_2 | SCS | CAP;
        //         while ((TA2CCTL2 & CCIFG) == 0)
        //             ;
        //         x = TA2CCR2;
        //         TA2CCTL2 &= ~CCIFG;

        //         while ((TA2CCTL2 & CCIFG) == 0)
        //             ;
        //         y = TA2CCR2;
        //         TA2CCR2 &= ~CCIFG;
        //         dif = y - x;
        //         if (dif < 0)
        //         {
        //             dif += 65536;
        //         }

        //         tempo[i] = dif;
        //     }
        //     while ((UCA0IFG & UCTXIFG) == 0);// Esperar TXIFG=1

        //     char texto[100];
        //     int umi;
        //     umi = tempo[0]
        //     texto = ""
        //     UCA0TXBUF = ; // Próxima letra
        //     if (letra == 'Z' + 1)
        //         letra = '\n'; // Nova linha
        //     if (letra == '\n' + 1)
        //         letra = 'A'; // Volta ao 'A'
    }
    return 0;
}

void io_config(void)
{

    P2DIR &= ~BIT0;
    P2SEL |= BIT0;
}
void ta2_config(void)
{
    TA2CTL = TASSEL__SMCLK | MC_2 | TACLR;
}

// Configurar USCI_A0
void USCI_A0_config(void)
{
    UCA0CTL1 = UCSWRST;   // RST=1 para USCI_A0
    UCA0CTL0 = 0;         // sem paridade, 8 bits, 1 stop, modo UART
    UCA0BRW = 3;          // Divisor
    UCA0MCTL = UCBRS_3;   // Modulador = 3 e UCOS=0
    P3SEL |= BIT4 | BIT3; // Disponibilizar pinos
    UCA0CTL1 = UCSSEL_1;  // RST=0, ACLK
}

void adc_config(void)
{
    ADC12CTL0 = 0;                      // Disable
    ADC12CTL0 |= ADC12SHT0_3 | ADC12ON; // Escolha do SHT e ligando adc

    ADC12CTL1 = ADC12CONSEQ_2 | ADC12SHS_0 | ADC12CSTARTADD_0 | ADC12SSEL_0 | ADC12SHP;

    ADC12CTL2 = ADC12RES_0; // 8 bits
    ADC12MCTL0 = ADC12SREF_0 | ADC12INCH_0;

    P6SEL |= BIT0; // Desligando saida digital

    ADC12CTL0 |= ADC12ENC; // Enable da conversao
}

void ser_char(char x)
{
    while ((UCA0IFG & UCTXIFG) == 0)
        ;
    UCA0TXBUF = x;
    __delay_cycles(1000);
}

void ser_str(char *x)
{
    char i = 0;
    while (x[i] != '\0')
        ser_char(x[i++]);
}

void ser_clr(void)
{
    ser_char(27);  // Send ESC character
    ser_char('['); // Send [
    ser_char('2'); // Send 2
    ser_char('J'); // Send J
}

void ser_dec8(char x)
{
    char aux;
    aux = x / 100;
    ser_char(0x30 + aux);
    x -= 100 * aux;
    aux = x / 10;
    ser_char(0x30 + aux);
    x -= 10 * aux;
    ser_char(0x30 + x);
}

void ser_dec16(char x)
{
    char aux;
    aux = x / 10000;
    if (aux != 0)
        ser_char(0x30 + aux);
    x -= 10000 * aux;

    aux = x / 1000;
    if (aux != 0)
        ser_char(0x30 + aux);
    x -= 1000 * aux;

    aux = x / 100;
    if (aux != 0)
        ser_char(0x30 + aux);
    x -= 100 * aux;

    aux = x / 10;
    if (aux != 0)
        ser_char(0x30 + aux);
    x -= 10 * aux;

    ser_char(0x30 + x);
}
