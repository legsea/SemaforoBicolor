#include "Header.h"

void controlTiempos(System_t *this)
{
    this->systemFlags.inteTimer = 0; //Limpio flag interrupcion

    this->systemFlags.leerEntradas = 1; //Levanto flag para leer entradas

    this->tiempos.t100mseg++; //para 100mseg de tiempo

    //A partir de aca la base de tiempos se establece en 100mseg
    if (this->tiempos.t100mseg == 10)
    {
        this->tiempos.t100mseg = 0; //Pasaron 100mseg

        if (this->tiempos.tarranque)
        {
            this->tiempos.tarranque--;

            if (this->tiempos.tarranque == 0)
            {
                this->systemFlags.arranque = 0;
            }
        }

        if (this->tiempos.tsave)
        {
            this->tiempos.tsave--;

            if (this->tiempos.tsave == 0)
            {
                this->systemFlags.save = 1;
            }
        }

        if (this->tiempos.tFreeze)
        {
            this->tiempos.tFreeze--;

            if (this->tiempos.tFreeze == 0)
            {
                this->systemFlags.freeze = 0;
            }
        }

        if (this->tiempos.tprog)
        {
            this->tiempos.tprog--;
            if (this->tiempos.tprog == 0)
                this->systemFlags.prog = 0;
        }

        this->tiempos.t1seg++;

        // Paso 1 segundo
        if (this->tiempos.t1seg == 10)
        {
            this->tiempos.t1seg = 0;
            this->systemFlags.refrescarDisplay = 1;
        }
    }
}

/* #define MASCARAPUERTOB 0X1F
void leerEntradasB(System_t *this)
{
    uint8_t auxiliarLectura;
    uint8_t diferencia;

    if (!this->systemFlags.primeraLecturaB)
    {
        this->systemFlags.primeraLecturaB = 1;            //Hago la primer lectura
        this->entradaB.entradas = PORTB & MASCARAPUERTOB; //Enmascaro estado entradas y guardo en.entradas
    }

    auxiliarLectura = PORTB & MASCARAPUERTOB;               //Enmascaro
    diferencia = this->entradaB.entradas - auxiliarLectura; // comparo con la primer muestra

    if (diferencia == 0) //Si dio cero, son iguales
    {
        this->contador.antirebote++;         //Incremento antirebote
        if (this->contador.antirebote == 30) //Pasaron como 300 mseg
        {
            this->contador.antirebote = 0;
            this->entradaB.entradas = auxiliarLectura;

            if (Optos)
                this->auxEntradaB.entradas = ~auxiliarLectura; //Transfiero lectura a los flags en entrada
            else
                this->auxEntradaB.entradas = auxiliarLectura; //Transfiero lectura a los flags en entrada
        }
    }
    else
    {
        auxiliarLectura = 0;
        diferencia = 0;
        this->systemFlags.primeraLecturaB = 0;
    }
}
*/

#define MASCARAPUERTOC 0XD0
void leerEntradasC(System_t *this)
{
    uint8_t auxiliarLectura;
    uint8_t diferencia;

    if (!this->systemFlags.primeraLectura)
    {
        this->systemFlags.primeraLectura = 1;            //Hago la primer lectura
        this->entrada.entradas = PORTC & MASCARAPUERTOC; //Enmascaro estado entradas y guardo en.entradas
    }

    auxiliarLectura = PORTC & MASCARAPUERTOC;              //Enmascaro
    diferencia = this->entrada.entradas - auxiliarLectura; // comparo con la primer muestra

    if (diferencia == 0) //Si dio cero, son iguales
    {
        this->contador.antirebote++;         //Incremento antirebote
        if (this->contador.antirebote == 30) //Pasaron como 300 mseg
        {
            this->contador.antirebote = 0;
            this->entrada.entradas = auxiliarLectura;
        }
    }
    else
    {
        auxiliarLectura = 0;
        diferencia = 0;
        this->systemFlags.primeraLectura = 0;
    }
}
