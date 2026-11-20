#ifndef DISPLAY_H
#define DISPLAY_H

//librarie pentru microcontrolerul nostru STM32F103CBT
#include "stm32f1xx_hal.h"
#include <stdint.h>

//Trebuie grupata initializarea, memoria de pixeli, comunicatia si machetele OLED intr-o singura clasa 

enum class EroareDisplay : uint8_t //lista de erori cu valori pe un octet
{
    NicioEroare = 0,            //Operatia care actualizeaza ultima eroare a reusit; zero reprezinta lipsa erorilor
    Neinitializat,              //Obiectul Display a fost creat, dar initialiazarea prin begin() nu a avut loc inca
    InitializareI2CEsuata,      //HAL_I2C nu a reusit sa configureze I2C pentru comunicarea cu display-ul
                                //Verificare configurare I2C1 si a pinilor PB6/PB7, acest cod nu spune singur cauza exacta
    DisplayFaraRaspuns,         //Nu s-a primit raspuns de la OLED
                                //Diferite erori: fire conectate gresit, lipsa alimentarii, alta adrese sau o problema pe magistrala I2C
    ComenziInvalide            //Comenzi gresite; Nu se trimite pachetul pentru ca sursa lipsest sau nu incape in tabloul rezervat comenzilor

}


class Display  
{
    public: // apelare din main sau clasele celorlalti colegi
        Display(); //constructor
        bool begin(); // configurare PB6/PB7 si I2C prin HAL, detectare OLED si trimitere initilizare in SSD1306
        bool isReady() const; //returnare initializarea/ultimul transfer a reusit, fara o verificare electronica
        const char* getLastError() const; //returnarea unui text scurt pentru debugger sau log. un OLED defect nu poate avertiza singur
        void setEnglish(bool enabled); //selectare numai texte englezesti/romanesti; nu navigheaza si nu salveaza alegerea in Flash
        bool setBrightness(uint8_t level); //primeste 0=minim, 1=mediu, 2=maxim. apelantul decide cand si de ce schimba nivelul

        void clear(); //sterge numai memoria de pixeli din RAM; 
        void drawPixel(int x, int y, bool on = true); //aprinde/stinge un pixel valid din buffer; implicit on=true
        



    private:
}

#endif