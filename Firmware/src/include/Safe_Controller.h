#pragma once
#include <stdint.h>

namespace safe 
{
    //Gruparea structurilor aautentificarilor
    //Campurile si valorile trebuiesc salvate de STORAGE
    struct SavedState{
        uint8_t version = 1; //Format initializat la 1
        uint8_t failures = 0;//Initializare nr de incercari la 3 greseli maxim, dupa 6 porneste alarma
        uint8_t groupFailures = 0;//Initializare progres greseli de cod (0..2, la a 3-a greseala se blocheaza)
        bool locked = false;      //Indicator blocare seif
        bool alarm = false;       //Initializare alarma
    };
}