#ifndef DISPLAY_H
#define DISPLAY_H

//librarie pentru microcontrolerul nostru STM32F103CBT
#include "stm32f1xx_hal.h"
#include <stdint.h>

//Trebuie grupata initializarea, memoria de pixeli, comunicatia si machetele OLED intr-o singura clasa 

class Display  
{
    public: // apelare din main sau clasele celorlalti colegi
        Display(); //constructor
        bool begin(); // configurare PB6/PB7 si I2C prin HAL, detectare OLED si trimitere initilizare in SSD1306
        bool isReady() const; //returnare initializarea/ultimul transfer a reusit, fara o verificare electronica




    private:
}