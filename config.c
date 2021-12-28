#include "Header.h"

void configDevice(void)
{
    configPorts();
    configInterrupts();
    configTimers();
    configADC();
    configCCP();
    configComparadores();
    configOscilador();
}

void configPorts()
{
    // Puerto A

    PORTA = 0x00;
    TRISA = 0b00000000;
    ANSEL = 0x00;
    ANSELH = 0x00;

    // Puerto B
    nRBPU = 0; //Pull ups enable
    PORTB = 0x00;
    TRISB = 0b00000001;

    WPUB = 0xFF;

    // Puerto C
    PORTC = 0x00;
    TRISC = 0b11010000;
}

void configInterrupts(void)
{
    GIE = 1;  // Interrupciones globales
    PEIE = 1; // Interrupciones de perifericos
    T0IE = 0; // Interrupciones por Timer0
    INTE = 0; // Interrupciones por flanco
    RBIE = 0; // Interrupcion por RB
    T0IF = 0;
    INTF = 0;
    RBIF = 0;
    INTEDG = 1;
}

void configCCP(void)
{
    // Configuracion de CCP
    CCP1IE = 0; // Interrupciones por CCP (desactivado)
    CCP2IE = 0;
    CCP1CON = 0x00;
    CCP2CON = 0x00;
}

void configTimers(void)
{
    // Configuracion de Timer
    // Timer0
    T0CS = 0; // Contador en el pin T0CKI = 1; Reloj interno = 0;
    T0SE = 1; // Transicion de bajo a alto = 0;
    PSA = 0;  // Pre escaler asignado a: 0=TMR0/1=WDT;
    PS2 = 0;
    PS1 = 0; // Pre escaler 1:4
    PS0 = 1;
    TMR0 = 5; //Para interrupciones cada 500useg@8Mhz // 255-5=250*4=1000*.5useg=500useg

    // Timer1
    T1CKPS1 = 0; // Pre escaler 1:1
    T1CKPS0 = 0; // Pre escaler 1:1
    T1OSCEN = 0; //
    nT1SYNC = 1; //
    TMR1CS = 0;  // Timer1 como reloj interno
    TMR1IE = 1;  // Interrupciones por Timer1
    TMR1ON = 1;  // Timer 1 encendido
    TMR1H = 0xB1;
    TMR1L = 0xDF;

    // Timer2
    TMR2IE = 1; // Interrupciones por Timer2
    TOUTPS3 = 0;
    TOUTPS2 = 0;
    TOUTPS1 = 0; //Bits de Post escaler
    TOUTPS0 = 0;
    TMR2ON = 1; //Enciendo el timer2
    T2CKPS1 = 1;
    T2CKPS0 = 1; // Pre escaler 1:4
    TMR2 = 1;
    PR2 = 110; //Para que interrumpa cada 1mseg aproximadamente
}

void configComparadores(void)
{
    // Comparadores

    CM1CON0 = 0x00;
    CM2CON0 = 0x00;
}

void configOscilador(void)
{
    OSCCON = 0b01110001; //Pic trabajando con Osc. Interno a 8 Mhz
}

void configADC(void)
{
    // ADC

    ADCON0 = 0b10000000; //
    ADCON1 = 0b10000000;

    VCFG1 = 0;
    VCFG0 = 0;

    CHS0 = 0;
    CHS1 = 0;
    CHS2 = 0;
    CHS3 = 0;
}