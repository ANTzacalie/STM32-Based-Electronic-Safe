
// Mario, include path la Display.h , il tot stergi, cum vede clasa unde implementezi metodele?
#include "Display.h"
#include <string.h>

Display::Display()
{
    //Trebuiesc initializate periferice dar nu inainte de HAL_Init
}

bool Display::begin()
{
    ready_ = false; //invalidare a vechii confirmari de comunicare, inclusiv la o reinitializare ceruta
    address_ = 0; //resetare adresa memorata pentru a repeta detectarea modulului
}