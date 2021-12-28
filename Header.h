#ifndef _HEADER_H
#define _HEADER_H

#include <pic.h>
#include <stdint.h>
#include <pic16f886.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "16F886.h"

#define EEPROM_SAVE 0x05

typedef struct
{
    uint8_t multiplexado;
    uint8_t numeroapuerto;
    uint8_t unidad;
    uint8_t decena;
    uint8_t centena;
    uint8_t puestoUnidad;
    uint8_t puestoDecena;
} Display_t;

typedef union
{
    uint8_t genericFlags;
    struct
    {
        unsigned inteTimer : 1;
        unsigned leerEntradas : 1;
        unsigned primeraLectura : 1;
        unsigned refrescarDisplay : 1;
        unsigned arranque : 1;
        unsigned prog : 1;
        unsigned save : 1;
        unsigned freeze : 1;
    };

} SystemFlags_t;

typedef union
{
    uint8_t flags;
    struct
    {
        // unsigned refrescarDisplay : 1;
        unsigned color : 1;     // 0:Rojo - 1:Verde
        unsigned startStop : 1; // 0:Stop - 1:Start
        unsigned SSScolor : 1;  // Arranca,pero sin cambiar de color
    };

} Flags_t;

typedef union
{
    uint8_t entradas;
    struct
    {
        unsigned NU : 4;
        unsigned startR : 1; // RC4
        unsigned NU1 : 1;
        unsigned startV : 1;
        unsigned prog : 1;
    };

} Entrada_t;

typedef union
{
    uint16_t twoBytes;
    struct
    {
        unsigned low : 8;
        unsigned high : 8;
    };

} Split_t;

typedef struct
{
    uint8_t t500useg;
    uint8_t t100mseg;
    uint8_t loco;
    uint8_t t1seg;
    uint8_t tarranque;
    uint8_t tprog;
    uint8_t tsave;
    uint8_t tFreeze;
} Timers_t;

typedef struct
{
    uint8_t antirebote;
} Contadores_t;

typedef struct
{
    SystemFlags_t systemFlags;
    Timers_t tiempos;
    Entrada_t entrada;
    Contadores_t contador;

} System_t;

System_t core;

typedef struct
{
    uint8_t estado;
    uint8_t contador;
    Display_t display;
    Flags_t flags;

} Semaforo_t;

#include "funciones.h"
#include "display.h"

#endif //  _HEADER_H
