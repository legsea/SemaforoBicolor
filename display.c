#include "Header.h"

void Display(Semaforo_t *sema)
{

    sema->display.decena = (sema->contador) / 10;                        //Saca decena
    sema->display.unidad = (sema->contador - sema->display.decena * 10); //Saca unidades

    //Apaga los digitos si estan en cero

    if (sema->display.decena == 0)
        sema->display.decena = 20; // Si es cero, estan apagados
}

void var2num(uint8_t temp)
{
    switch (temp)
    {
    //    		  fpgabcde
    case 0:
        PORTA = 0b10011111; //Muestra un Cero
        break;
    case 1:
        PORTA = 0b00001100; //Muestra un Uno
        break;
    case 2:
        PORTA = 0b00111011;
        break;
    case 3:
        PORTA = 0b00111110;
        break;
    case 4:
        PORTA = 0b10101100;
        break;
    case 5:
        PORTA = 0b10110110;
        break;
    //    		  fpgabcde
    case 6:
        PORTA = 0b10110111;
        break;
    case 7:
        PORTA = 0b00011100;
        break;
    case 8:
        PORTA = 0b10111111;
        break;
    case 9:
        PORTA = 0b10111100;
        break;
    //			  fpgabcde
    case 10:
        PORTA = 0b10111101; //A
        break;
    case 11:
        PORTA = 0b10100111; //b
        break;
    case 12:
        PORTA = 0b10110011; //C
        break;
    case 13:
        PORTA = 0b00101111; //d
        break;
    case 14:
        PORTA = 0b10110011; //E
        break;
    case 15:
        PORTA = 0b10110001; //F
        break;
    case 16:
        PORTA = 0b00100000; // - (Guion)
        break;
    case 20:
        PORTA = 0b00000000; //Nada
        break;
    }
}
