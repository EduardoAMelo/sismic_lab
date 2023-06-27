#include <msp430.h>

#define TRUE 1
#define FALSE 0
#define MAX 2621
#define MIN 524

volatile int x, y;
volatile float vx, vy;

// Defini��o do endere�o do PCF_8574
#define PCF_ADR1 0x3F
#define PCF_ADR2 0x27
#define PCF_ADR PCF_ADR2

#define BR_100K 11 // SMCLK/100K = 11
#define BR_50K 21  // SMCLK/50K  = 21
#define BR_10K 105 // SMCLK/10K  = 105

#define ABERTA 1  // Chave aberta
#define FECHADA 0 // Chave fechada
#define DBC 1000  // Sugestão para o debounce

void lcd_inic(void);
void lcd_aux(char dado);
int pcf_read(void);
void pcf_write(char dado);
int pcf_teste(char adr);
void led_vd(void);
void led_VD(void);
void led_vm(void);
void led_VM(void);
void i2c_config(void);
void gpio_config(void);
void delay(long limite);
void lcd_char(char x);
void lcd_str(char *pt);
void lcd_cursor(char x);
void lcd_dec8(char z);
void ADC_config(void);
void ta0_config(void);
void ta2_config(void);
void debounce(int valor);
int mon_sw(void);
void lcd_float16(float value);
void lcd_dec16(int z);


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    gpio_config();
    i2c_config();
    ta0_config();
    ADC_config();
    ta2_config();

    int sent = 0;

    // Teste do LCD
    if (pcf_teste(PCF_ADR) == FALSE)
    {
        led_VM(); // Indicar que n�o houve ACK
        while (TRUE)
            ; // Travar
    }
    else
        led_VD(); // Houve ACK, tudo certo

    lcd_inic();   // Inicializar LCD
    pcf_write(8); // Acender Back Light
    int carga = MIN;
    char modo = 0;
    // // Escrever  letra A = 0x41
    // lcd_char('a');
    // lcd_cursor(0x42);
    // lcd_str("batata");
    // lcd_dec8(123);
    while (TRUE)
    {
        __delay_cycles(1000);
        while ((ADC12IFG & ADC12IFG7) == 0)
            ;
        x = ADC12MEM0 + ADC12MEM2 + ADC12MEM4 + ADC12MEM6;
        y = ADC12MEM1 + ADC12MEM3 + ADC12MEM5 + ADC12MEM7;
        x /= 4;
        y /= 4;
        vx = (3.3 / 4095) * x;
        vy = (3.3 / 4095) * y;
        if (mon_sw() == TRUE)
            modo = modo^1;
        if (modo == 0){
            TA2CCR2 = (2097 * (vx / 5)) + 524;
            lcd_cursor(0);
            lcd_char('A');
            lcd_dec8(1);
            lcd_char('=');
            lcd_float16(vx);
            lcd_char('V');
            lcd_str("   ");
            lcd_dec16(x);
        } else {
            TA2CCR2 = (2097 * (vy / 5)) + 524;
            lcd_cursor(0);
            lcd_char('A');
            lcd_dec8(2);
            lcd_char('=');
            lcd_float16(vy);
            lcd_char('V');
            lcd_str("   ");
            lcd_dec16(y);
        }

    }

    ; // Travar execu��o
    return 0;
}

// Incializar LCD modo 4 bits
void lcd_inic(void)
{

    // Preparar I2C para operar
    UCB0I2CSA = PCF_ADR; // Endere�o Escravo
    UCB0CTL1 |= UCTR |   // Mestre TX
                UCTXSTT; // Gerar START
    while ((UCB0IFG & UCTXIFG) == 0)
        ;          // Esperar TXIFG=1
    UCB0TXBUF = 0; // Sa�da PCF = 0;
    while ((UCB0CTL1 & UCTXSTT) == UCTXSTT)
        ;                                   // Esperar STT=0
    if ((UCB0IFG & UCNACKIFG) == UCNACKIFG) // NACK?
        while (1)
            ;

    // Come�ar inicializa��o
    lcd_aux(0); // RS=RW=0, BL=1
    delay(20000);
    lcd_aux(3); // 3
    delay(10000);
    lcd_aux(3); // 3
    delay(10000);
    lcd_aux(3); // 3
    delay(10000);
    lcd_aux(2); // 2

    // Entrou em modo 4 bits
    lcd_aux(2);
    lcd_aux(8); // 0x28
    lcd_aux(0);
    lcd_aux(8); // 0x08
    lcd_aux(0);
    lcd_aux(1); // 0x01
    lcd_aux(0);
    lcd_aux(6); // 0x06
    lcd_aux(0);
    lcd_aux(0xF); // 0x0F

    while ((UCB0IFG & UCTXIFG) == 0)
        ;                // Esperar TXIFG=1
    UCB0CTL1 |= UCTXSTP; // Gerar STOP
    while ((UCB0CTL1 & UCTXSTP) == UCTXSTP)
        ; // Esperar STOP
    delay(50);
}

// Auxiliar inicializa��o do LCD (RS=RW=0)
// *** S� serve para a inicializa��o ***
void lcd_aux(char dado)
{
    while ((UCB0IFG & UCTXIFG) == 0)
        ;                                    // Esperar TXIFG=1
    UCB0TXBUF = ((dado << 4) & 0XF0) | BIT3; // PCF7:4 = dado;
    delay(50);
    while ((UCB0IFG & UCTXIFG) == 0)
        ;                                           // Esperar TXIFG=1
    UCB0TXBUF = ((dado << 4) & 0XF0) | BIT3 | BIT2; // E=1
    delay(50);
    while ((UCB0IFG & UCTXIFG) == 0)
        ;                                    // Esperar TXIFG=1
    UCB0TXBUF = ((dado << 4) & 0XF0) | BIT3; // E=0;
}

// Ler a porta do PCF
int pcf_read(void)
{
    int dado;
    UCB0I2CSA = PCF_ADR; // Endere�o Escravo
    UCB0CTL1 &= ~UCTR;   // Mestre RX
    UCB0CTL1 |= UCTXSTT; // Gerar START
    while ((UCB0CTL1 & UCTXSTT) == UCTXSTT)
        ;
    UCB0CTL1 |= UCTXSTP; // Gerar STOP + NACK
    while ((UCB0CTL1 & UCTXSTP) == UCTXSTP)
        ; // Esperar STOP
    while ((UCB0IFG & UCRXIFG) == 0)
        ; // Esperar RX
    dado = UCB0RXBUF;
    return dado;
}

// Escrever dado na porta
void pcf_write(char dado)
{
    UCB0I2CSA = PCF_ADR; // Endere�o Escravo
    UCB0CTL1 |= UCTR |   // Mestre TX
                UCTXSTT; // Gerar START
    while ((UCB0IFG & UCTXIFG) == 0)
        ;             // Esperar TXIFG=1
    UCB0TXBUF = dado; // Escrever dado
    while ((UCB0CTL1 & UCTXSTT) == UCTXSTT)
        ;                                   // Esperar STT=0
    if ((UCB0IFG & UCNACKIFG) == UCNACKIFG) // NACK?
        while (1)
            ;            // Escravo gerou NACK
    UCB0CTL1 |= UCTXSTP; // Gerar STOP
    while ((UCB0CTL1 & UCTXSTP) == UCTXSTP)
        ; // Esperar STOP
}

// Testar endere�o I2C
// TRUE se recebeu ACK
int pcf_teste(char adr)
{
    UCB0I2CSA = adr;            // Endere�o do PCF
    UCB0CTL1 |= UCTR | UCTXSTT; // Gerar START, Mestre transmissor
    while ((UCB0IFG & UCTXIFG) == 0)
        ;                // Esperar pelo START
    UCB0CTL1 |= UCTXSTP; // Gerar STOP
    while ((UCB0CTL1 & UCTXSTP) == UCTXSTP)
        ; // Esperar pelo STOP
    if ((UCB0IFG & UCNACKIFG) == 0)
        return TRUE;
    else
        return FALSE;
}

// Configurar UCSB0 e Pinos I2C
// P3.0 = SDA e P3.1=SCL
void i2c_config(void)
{
    UCB0CTL1 |= UCSWRST;  // UCSI B0 em ressete
    UCB0CTL0 = UCSYNC |   // S�ncrono
               UCMODE_3 | // Modo I2C
               UCMST;     // Mestre
    UCB0BRW = BR_100K;    // 100 kbps
    P3SEL |= BIT1 | BIT0; // Use dedicated module
    UCB0CTL1 = UCSSEL_2;  // SMCLK e remove ressete
}

void led_vd(void) { P4OUT &= ~BIT7; } // Apagar verde
void led_VD(void) { P4OUT |= BIT7; }  // Acender verde
void led_vm(void) { P1OUT &= ~BIT0; } // Apagar vermelho
void led_VM(void) { P1OUT |= BIT0; }  // Acender vermelho

// Configurar leds
void gpio_config(void)
{
    P1DIR |= BIT0;  // Led vermelho
    P1OUT &= ~BIT0; // Vermelho Apagado
    P4DIR |= BIT7;  // Led verde
    P4OUT &= ~BIT7; // Verde Apagado

    // Config do Servo
    P2OUT &= ~BIT5;
    P2DIR |= BIT5;
    P2SEL |= BIT5;

    P6DIR &= ~BIT3;
    P6OUT |= BIT3;
    P6REN |= BIT3;

    // P2DIR &= ~BIT0;
    // P2SEL |= BIT0;
}

void delay(long limite)
{
    volatile long cont = 0;
    while (cont++ < limite)
        ;
}

void lcd_char(char x)
{
    char esq, dir;
    esq = x & 0xF0;
    dir = (x & 0x0F) << 4;

    pcf_write(esq | 9);
    pcf_write(esq | 0xD);
    pcf_write(esq | 9);
    pcf_write(dir | 9);
    pcf_write(dir | 0xD);
    pcf_write(dir | 9);
}
void lcd_str(char *pt)
{
    char i = 0;
    while (pt[i] != 0)
        lcd_char(pt[i++]);
}

void lcd_cursor(char x)
{
    x |= 0x80;
    char esq, dir;
    esq = x & 0xF0;
    dir = (x & 0x0F) << 4;

    pcf_write(esq | 8);
    pcf_write(esq | 0xC);
    pcf_write(esq | 8);
    pcf_write(dir | 8);
    pcf_write(dir | 0xC);
    pcf_write(dir | 8);
}


void lcd_dec8(char z)
{
    char firstDigitDisplayed = 0; // Flag to track the first non-zero digit displayed

    char hundreds = z / 100;  // Hundreds
    if (hundreds > 0 || firstDigitDisplayed)
    {
        lcd_char(hundreds + 48);
        firstDigitDisplayed = 1;
    }

    char tens = (z % 100) / 10;  // Tens
    if (tens > 0 || firstDigitDisplayed)
    {
        lcd_char(tens + 48);
        firstDigitDisplayed = 1;
    }

    char units = z % 10;  // Units
    lcd_char(units + 48);
}

void lcd_dec16(int z)
{
    char firstDigitDisplayed = 0; // Flag to track the first non-zero digit displayed

    int tenThousands = z / 10000;  // Ten Thousands
    if (tenThousands > 0 || firstDigitDisplayed)
    {
        lcd_char(tenThousands + 48);
        firstDigitDisplayed = 1;
    }

    int thousands = (z % 10000) / 1000;  // Thousands
    if (thousands > 0 || firstDigitDisplayed)
    {
        lcd_char(thousands + 48);
        firstDigitDisplayed = 1;
    }

    int hundreds = (z % 1000) / 100;  // Hundreds
    if (hundreds > 0 || firstDigitDisplayed)
    {
        lcd_char(hundreds + 48);
        firstDigitDisplayed = 1;
    }

    int tens = (z % 100) / 10;  // Tens
    if (tens > 0 || firstDigitDisplayed)
    {
        lcd_char(tens + 48);
        firstDigitDisplayed = 1;
    }

    int units = z % 10;  // Units
    lcd_char(units + 48);
}

// ADC CONFIG
void ADC_config(void)
{
    ADC12CTL0 &= ~ADC12ENC; // Desabilitar para configurar
    ADC12CTL0 = ADC12ON;    // Ligar ADC

    ADC12CTL1 = ADC12CONSEQ_3 |    // Modo sequência de canais
                ADC12SHS_1 |       // Selecionar TA0.1
                ADC12CSTARTADD_0 | // Resultado em ADC12MEM0
                ADC12SSEL_3;       // ADC12CLK = SMCLK

    ADC12CTL2 = ADC12RES_2;                            // ADC12RES=0, Modo 8 bIts
    ADC12MCTL0 = ADC12SREF_0 | ADC12INCH_1;            // Config MEM1
    ADC12MCTL1 = ADC12SREF_0 | ADC12INCH_2;            // MEM2 = última
    ADC12MCTL2 = ADC12SREF_0 | ADC12INCH_1;            // Config MEM1
    ADC12MCTL3 = ADC12SREF_0 | ADC12INCH_2;            // MEM2 = última
    ADC12MCTL4 = ADC12SREF_0 | ADC12INCH_1;            // Config MEM1
    ADC12MCTL5 = ADC12SREF_0 | ADC12INCH_2;            // MEM2 = última
    ADC12MCTL6 = ADC12SREF_0 | ADC12INCH_1;            // Config MEM1
    ADC12MCTL7 = ADC12SREF_0 | ADC12INCH_2 | ADC12EOS; // MEM2 = última
    P6SEL |= BIT2 | BIT1;                              // Desligar digital de P6.2,1
    ADC12CTL0 |= ADC12ENC;                             // Habilitar ADC12
}
void ta0_config(void)
{
    TA0CTL = TASSEL__ACLK | MC__UP;
    TA0CCR0 = 1024;
    TA0CCTL1 = OUTMOD_6;
    TA0CCR1 = TA0CCR0 / 2;
}

void ta2_config(void)
{
    TA2CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA2CCTL2 = OUTMOD_6;
    TA2CCR0 = 20971;
    TA2CCR2 = 1049;
}

int mon_sw(void)
{
    static int ps1 = ABERTA; // Guardar passado de S1
    if ((P6IN & BIT3) == 0)
    { // Qual estado atual de S1?
        if (ps1 == ABERTA)
        { // Qual o passado de S1?
            debounce(DBC);
            ps1 = FECHADA;
            return TRUE;
        }
    }
    else
    {
        if (ps1 == FECHADA)
        { // Qual o passado de S1?
            debounce(DBC);
            ps1 = ABERTA;
            return FALSE;
        }
    }
    return FALSE;
}

void debounce(int valor)
{
    volatile int x; // volatile evita optimizador do compilador
    for (x = 0; x < valor; x++)
        ; // Apenas gasta tempo
}

void lcd_float16(float value)
{
    int integer = (int)value;
    float fraction = value - integer;

    lcd_dec8(integer);       // Display integer part

    lcd_char('.');           // Display decimal point

    int decimals = (int)(fraction * 10000);  // Get the first 4 decimal places
    lcd_dec8(decimals);      // Display decimal places
}
