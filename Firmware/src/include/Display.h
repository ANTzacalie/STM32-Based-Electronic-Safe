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
    ComenziInvalide             //Comenzi gresite; Nu se trimite pachetul pentru ca sursa lipsest sau nu incape in tabloul rezervat comenzilor

};


class Display  
{
    public: // apelare din main sau clasele celorlalti colegi
        //METODA 01 - Display: definitia este in src/Display.cpp, la acelasi numar
        Display(); //constructor

        //METODA 02 - begin: definitia este in src/Display.cpp, la acelasi numar
        bool begin(); // configurare PB6/PB7 si I2C prin HAL, detectare OLED si trimitere initilizare in SSD1306

        //METODA 03 - isReady: definitia este in src/Display.cpp, la acelasi numar
        bool isReady() const; //returnare initializarea/ultimul transfer a reusit, fara o verificare electronica

        //METODA 04 - getLastError: definitia este in src/Display.cpp, la acelasi numar
        EroareDisplay getLastError() const; //returnareza printr-o valoare eroarea din lista EroareDisplay, fara pointer sau text. un OLED defect nu poate avertiza singur

        //METODA 05 - setEnglish: definitia este in src/Display.cpp, la acelasi numar
        void setEnglish(bool enabled); //selectare numai texte englezesti/romanesti; nu navigheaza si nu salveaza alegerea in Flash

        //METODA 06 - setBrightness: definitia este in src/Display.cpp, la acelasi numar
        bool setBrightness(uint8_t level); //primeste 0=minim, 1=mediu, 2=maxim. apelantul decide cand si de ce schimba nivelul

        //METODA 07 - clear: definitia este in src/Display.cpp, la acelasi numar
        void clear(); //sterge numai memoria de pixeli din RAM; 

        //METODA 08 - drawPixel: definitia este in src/Display.cpp, la acelasi numar
        void drawPixel(int x, int y, bool on = true); //aprinde/stinge un pixel valid din buffer; implicit on=true

        //METODA 09 - drawText: definitia este in src/Display.cpp, la acelasi numar
        bool update(); //trimite paginile modificate catre OLED; false inseamna ca OLED-ul nu a raspuns sau a esuat transferul
        



    private:
        I2C_HandleTypeDef i2c = {};   //Pregateste handle-ul de configurare cerut de HAL, cu toate campurile initial zero
        uint16_t address = 0; //Adresa OLED in format HAL; se stabileste dupa raspuns la 0x3C sau 0x3D pe 7 biti
        uint8_t dirtyPages = 0; //Ceii patru biti inferiori indica paginile de cate 8 linii care trebuie retransmise
        bool ready = false; //confirmare de comunicatie reusita, inclusiv dupa transmiterea secventei de configurare
        EroareDisplay lastError = EroareDisplay::Neinitializat; //ultima eroare de comunicatie sau de parametrii primiti de metodele de afisare
        bool sendCommands(const uint8_t* commands, uint8_t length); //trimite comenzi SSD1306 si returneaza succesul transferului (cel mult 31 de comenzi/parametrii SSD1306 cu prefixul de control 0x00)
        bool english = false; //Porneste cu texte in romana; reprezinta doar preferinta vizuala curenta, nu un meniu sau o salvare in Flash

};

#endif