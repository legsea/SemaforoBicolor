/**
 * @file main.c
 * @author Ing. Martin Leg (you@domain.com)
 * @brief Control de un semaforoforo bicolor
 * @version 0.1
 * @date 2021-12-01
 *
 * @copyright Copyright (c) 2021
 *
 *  Placa Turnero SEA 222/19
 */

#include "Header.h"

__CONFIG(FOSC_INTRC_NOCLKOUT &WDTE_OFF &PWRTE_ON &MCLRE_OFF &CP_ON &CPD_ON &BOREN_ON &IESO_OFF &FCMEN_ON &LVP_OFF);
__CONFIG(BOR4V_BOR40V &WRT_OFF);

Semaforo_t semaforo;

void interrupt interrupciones(void)
{

    if (TMR1IE && TMR1IF) // Preparado para interrumpir cada 10mseg
    {

        TMR1IF = 0;

        core.systemFlags.inteTimer = 1;

        TMR1H = 0xB1;
        TMR1L = 0xDF;
    }

    if (TMR2IE && TMR2IF) // Interrupciones para display
    {
        semaforo.display.multiplexado++;

        if (semaforo.display.multiplexado == 6)
            semaforo.display.multiplexado = 1;

        pinDecenas = 0;
        pinUnidades = 0;

        if (semaforo.display.multiplexado < 3)
        {

            switch (semaforo.display.multiplexado)
            {
            case 1:
                var2num(semaforo.display.unidad);
                pinUnidades = 1;
                break;

            case 2:
                var2num(semaforo.display.decena);
                pinDecenas = 1;
                break;

                /*             case 3:
                var2num(semaforo.display.centena);
                digito3 = 1;
                break; */
            }
        }
        TMR2IF = 0;
    }
}

void main(void)
{
    configDevice();

    semaforo.contador = EEPROM_READ(EEPROM_SAVE);

    if (semaforo.contador > 60)
        semaforo.contador = 60;

    core.systemFlags.arranque = 1;
    core.tiempos.tarranque = 10;
    Contando = 1;

    while (core.systemFlags.arranque)
    {
        if (core.systemFlags.inteTimer)
        {
            controlTiempos(&core);
        }
    }

    while (1)
    {
        if (core.systemFlags.inteTimer)
        {
            controlTiempos(&core);
        }

        if (core.systemFlags.leerEntradas) // Lectura entradas
        {
            core.systemFlags.leerEntradas = 0;
            leerEntradasC(&core);
        }

        if (core.systemFlags.refrescarDisplay) // Cada 1 segundo
        {
            core.systemFlags.refrescarDisplay = 0;

            if (semaforo.flags.startStop == 1)
            {
                semaforo.contador--;
            }

            Display(&semaforo);

            if (semaforo.contador == 0)
            {
                semaforo.flags.startStop = 0;
                releColor = 0;
                Contando = 1;
                semaforo.contador = EEPROM_READ(EEPROM_SAVE);
                core.systemFlags.freeze = 1;
                core.tiempos.tFreeze = 20;
            }
        }

        if (core.systemFlags.freeze == 0)
        {
            if (core.entrada.startV == 0 && Contando == 1)
            {
                semaforo.flags.startStop = 1;
                releColor = 1;
                core.tiempos.t1seg = 0; // Reseteo contador
                Contando = 0;
            }

            if (core.entrada.startR == 0 && Contando == 1)
            {
                semaforo.flags.startStop = 1;
                releColor = 0;
                core.tiempos.t1seg = 0; // Reseteo contador
                Contando = 0;
            }
        }

        if (core.entrada.prog == 0 && core.systemFlags.prog == 0)
        {
            semaforo.contador++;

            if (semaforo.contador > 60)
                semaforo.contador = 10;

            Display(&semaforo);
            core.systemFlags.prog = 1;
            core.tiempos.tprog = 7;
            core.tiempos.tsave = 20;
        }

        if (core.systemFlags.save)
        {
            core.systemFlags.save = 0;
            EEPROM_WRITE(EEPROM_SAVE, semaforo.contador);
        }
    }
}
