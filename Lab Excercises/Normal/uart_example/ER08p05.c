#include <msp430.h>

#define TRUE 1
#define FALSE 0

void usci_a1_config(void);
void ser_char(char x);
void ser_str(char *x);
void ser_dec8(char x);
void ser_dec16(unsigned int x);
void ser_crlf(void);

int main(void)
{
    char letra;
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    usci_a1_config();

    __delay_cycles(1000);
    int cont = 0;
    ser_dec16(60000);

    while (1)
    {
        ser_dec16(cont++);
        ser_crlf();
        __delay_cycles(1000);
    }
    letra = 'A';
    while (TRUE)
    { // La�o principal
        while ((UCA1IFG & UCTXIFG) == 0)
            ;                // Esperar TXIFG=1
        UCA1TXBUF = letra++; // Pr�xima l
        if (letra == 'Z' + 1)
            letra = '\n'; // Nova linha
        if (letra == '\n' + 1)
            letra = '\r'; // Volta ao 'A'
        if (letra == '\r' + 1)
            letra = 'A'; // Volta ao 'A'
        __delay_cycles(1000);
    }
    return 0;
}

// USCI_A1: Configurar Serial 1 (MSP <=> PC)
// P4.4 = TX (MSP ==> PC)
// P4.5 = RX (MSP <== PC)
void usci_a1_config(void)
{
    UCA1CTL1 = UCSSEL_2 |         // SMCLK=1.048.576Hz
               UCSWRST;           // UCSI em Reset
    UCA1CTL0 = 0;                 // 1 Stop
    UCA1BRW = 109;                //
    UCA1MCTL = UCBRS_2 | UCBRF_0; // UCOS16=0

    P4DIR |= BIT4;        // P4.4 sa�da
    P4DIR &= ~BIT5;       // P4.5 entrada
    P4SEL |= BIT5 | BIT4; // Selecionar UART

    PMAPKEYID = 0X02D52; // Liberar mapeamentp
    P4MAP4 = PM_UCA1TXD; // TX=Sair por P4.4
    P4MAP5 = PM_UCA1RXD; // RX=Receber por P4.5

    UCA1CTL1 &= ~UCSWRST; // UCSI sai de Reset
}

void ser_char(char x)
{
    while ((UCA1IFG & UCTXIFG) == 0)
        ;
    UCA1TXBUF = x;
}

void ser_str(char *x)
{
    char i = 0;
    while (x[i] != '\0')
        ser_char(x[i++]);
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

void ser_dec16(unsigned int z)
{
    volatile int aux;
    volatile unsigned int x = z;
    aux = x / 10000;
    ser_char(0x30 + aux);
    x -= 10000 * aux;
    aux = x / 1000;
    ser_char(0x30 + aux);
    x -= 1000 * aux;
    aux = x / 100;
    ser_char(0x30 + aux);
    x -= 100 * aux;
    aux = x / 10;
    ser_char(0x30 + aux);
    x -= 10 * aux;
    ser_char(0x30 + x);
}

void ser_crlf(void)
{
    ser_char(0xD);
    ser_char(0xA);
}
