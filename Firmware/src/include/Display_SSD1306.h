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
    ComenziInvalide,            //Comenzi gresite; Nu se trimite pachetul pentru ca sursa lipsest sau nu incape in tabloul rezervat comenzilor
    LuminozitateInvalida,       //setBrightness a primit alta valoare decat 0=minim, 1=mediu sau 2=maxim
    TransferPixeliEsuat,        //HAL nu a reusit sa trimita un pachet cu pixelii care trebuie afisati.
    DatePinInvalide,            //showPin a primit mai mult de 12 cifre de desenat sau mai mult de 3 incercari de afisat.   
    IncercariInvalide,          //showPinIncorrect a primit mai mult de 3 incercari ramase pentru mesajul de pe ecran.                         

    TransferComenziEsuat        //HAL nu a reusit sa trimita comenzile sau parametrii catre controlerul SSD1306.
                                //Se poate intampla la initializare, la schimbarea contrastului sau la selectarea paginii de pixeli.


};


class Display  
{
    public:  // apelare din main sau clasele celorlalti colegi
        //METODA 01 - Display: definitia este in src/Display.cpp, la acelasi numar
        Display();      //constructor

        //METODA 02 - begin: definitia este in src/Display.cpp, la acelasi numar
        bool begin();   // configurare PB6/PB7 si I2C prin HAL, detectare OLED si trimitere initilizare in SSD1306

        bool refreshPanel();  // Retrimite configuratia OLED si imaginea pastrata in memoria STM32.

        //METODA 03 - isReady: definitia este in src/Display.cpp, la acelasi numar
        bool isReady() const;  //returnare initializarea/ultimul transfer a reusit, fara o verificare electronica

        //METODA 04 - getLastError: definitia este in src/Display.cpp, la acelasi numar
        EroareDisplay getLastError() const;  //returnareza printr-o valoare eroarea din lista EroareDisplay, fara pointer sau text. un OLED defect nu poate avertiza singur

        //METODA 05 - setEnglish: definitia este in src/Display.cpp, la acelasi numar
        void setEnglish(bool enabled);       //selectare numai texte englezesti/romanesti; nu navigheaza si nu salveaza alegerea in Flash

        //METODA 06 - setBrightness: definitia este in src/Display.cpp, la acelasi numar
        bool setBrightness(uint8_t level);   //primeste 0=minim, 1=mediu, 2=maxim. apelantul decide cand si de ce schimba nivelul

        //METODA 07 - clear: definitia este in src/Display.cpp, la acelasi numar
        void clear();    //sterge numai memoria de pixeli din RAM; 

        //METODA 08 - drawPixel: definitia este in src/Display.cpp, la acelasi numar
        void drawPixel(int x, int y, bool on = true);   //aprinde/stinge un pixel valid din buffer; implicit on=true

        //METODA 09 - update: definitia este in src/Display.cpp, la acelasi numar
        bool update();   //trimite paginile modificate catre OLED; false inseamna ca OLED-ul nu a raspuns sau a esuat transferul
        
        //METODA 10 - drawText: definitia este in src/Display.cpp, la acelasi numar
        void drawText(int x, int y, const char* value);  //deseneaza un text in bufferul local de pixeli; nu trimite inca un cadru separat

        //METODA 11 - getBuffer: definitia este in src/Display.cpp, la acelasi numar
        const uint8_t* getBuffer() const; // Permite citirea celor 512 B in teste, fara a permite modificarea prin pointerul returnat.

        //METODA 12 - showMessage: definitia este in src/Display.cpp, la acelasi numar
        bool showMessage(const char* title, const char* row2, const char* row3, const char* row4); // Macheta generica de patru randuri, fiecare limitat vizual la 21 caractere.

        // METODA 13 - showHome: definitia este in src/Display.cpp, la acelasi numar.
        bool showHome(const char* status, const char* timeText, const char* dateText, bool batteryValid, uint8_t batteryPercent); // Afiseaza starea si ora/data primite; nu le calculeaza si nu confirma usa.

        // METODA 14 - showPin: definitia este in src/Display.cpp, la acelasi numar.
        bool showPin(uint8_t digitCount, uint8_t attemptsLeft, bool cursorVisible); // Afiseaza numai numarul de asteriscuri, incercarile si faza cursorului primite din exterior.

        // METODA 15 - showPinIncorrect: definitia este in src/Display.cpp, la acelasi numar.
        bool showPinIncorrect(uint8_t attemptsLeft); // Afiseaza mesajul de cod gresit si valoarea primita, fara decrementarea vreunui contor.

        // METODA 16 - showPinCorrect: definitia este in src/Display.cpp, la acelasi numar.
        bool showPinCorrect(); // Afiseaza rezultatul AUTH corect deja confirmat, fara a comanda deblocarea.

        // METODA 17 - showPinMismatch: definitia este in src/Display.cpp, la acelasi numar.
        bool showPinMismatch(); // Afiseaza ca doua introduceri ale unui PIN nou difera, fara sa le primeasca sau compare.

        // METODA 18 - showCardReading: definitia este in src/Display.cpp, la acelasi numar.
        bool showCardReading(); // Afiseaza citirea RFID in curs, fara acces SPI sau autorizare card.




    private:
        I2C_HandleTypeDef i2c = {};                                     //Pregateste handle-ul de configurare cerut de HAL, cu toate campurile initial zero
        uint16_t address = 0;                                           //Adresa OLED in format HAL; se stabileste dupa raspuns la 0x3C sau 0x3D pe 7 biti
        uint8_t dirtyPages = 0;                                         //Ceii patru biti inferiori indica paginile de cate 8 linii care trebuie retransmise
        uint8_t contrast = 0x01;                                        // Porneste la contrast minim si retine apoi ultima valoare trimisa cu succes.
        bool ready = false;                                             //confirmare de comunicatie reusita, inclusiv dupa transmiterea secventei de configurare
        EroareDisplay lastError = EroareDisplay::Neinitializat;         //ultima eroare de comunicatie sau de parametrii primiti de metodele de afisare
        bool sendCommands(const uint8_t* commands, uint8_t length);     //trimite comenzi SSD1306 si returneaza succesul transferului (cel mult 31 de comenzi/parametrii SSD1306 cu prefixul de control 0x00)
        bool english = false;                                           //Porneste cu texte in romana; reprezinta doar preferinta vizuala curenta, nu un meniu sau o salvare in Flash
        uint8_t buffer[512] = {};                                       //buffer local de pixeli, 128x32/8=512 octeti; un singur bit pentru fiecare pixel, initial toti pixelii sunt stinsi
        uint8_t fontColumn(char character, uint8_t column) const;       // Returneaza bitii unei coloane de caracter; nu acceseaza hardware.
        
        // METODA  - drawBattery: definitia este in src/Display.cpp, la acelasi numar.
        void drawBattery(bool valid, uint8_t percent); // Deseneaza pictograma din datele primite; procent invalid inseamna semnul intrebarii.

        // METODA  - drawMaskedPin: definitia este in src/Display.cpp, la acelasi numar.
        void drawMaskedPin(uint8_t digitCount, bool cursorVisible); // Deseneaza asteriscuri si cursor din lungime, fara sa primeasca PIN-ul real.
        
        const char* text(const char* romanian, const char* english) const; // Alege una dintre doua etichete constante dupa preferinta vizuala.


};

#endif