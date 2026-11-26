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

    TransferComenziEsuat,       //HAL nu a reusit sa trimita comenzile sau parametrii catre controlerul SSD1306.
                                //Se poate intampla la initializare, la schimbarea contrastului sau la selectarea paginii de pixeli.
    ParametruInvalid // Un rand, o lungime Password sau o valoare de afisat nu este permisa.

};


class Display  
{
    public:  // apelare din main sau clasele celorlalti colegi
        // METODA 01: Display - Creeaza obiectul si pregateste in RAM o imagine goala, cu octetul de control I2C. Nu configureaza pinii si nu trimite nimic pe OLED; hardware-ul este pregatit ulterior prin begin.
        Display();      //constructor

        // METODA 02: begin - Configureaza PB6/PB7 si I2C1, asteapta stabilizarea, cauta adresa OLED, trimite configurarea 128x32, afiseaza un cadru gol si porneste panoul. Returneaza true la succes sau false la primul esec, cu eroarea memorata.
        bool begin();   // configurare PB6/PB7 si I2C prin HAL, detectare OLED si trimitere initilizare in SSD1306
        
        // METODA 04: esteInitializat - Returneaza valoarea lui initializat: true daca afisajul este pregatit pentru comunicatie, false inainte de pornire sau dupa un defect de transfer. Nu verifica din nou cablurile si nu spune daca panoul este aprins.
        bool esteInitializat() const; // Arata daca OLED-ul a fost initializat si nu a aparut o eroare de comunicatie.

        // METODA 05: setEnglish - Retine true pentru texte in engleza sau false pentru romana. Alegerea se aplica mesajelor cerute ulterior; metoda nu redeseneaza imediat ecranul si nu salveaza limba in Flash.
        void setEnglish(bool engleza); // Alege limba urmatoarelor texte; nu proceseaza taste sau meniuri.
        
        // METODA 06: stergeBuffer - Pune zero in cei 512 octeti ai imaginii din RAM si pastreaza prefixul I2C pentru pixeli. Nu trimite nimic: textul deja vizibil pe OLED ramane pana cand se apeleaza afiseaza.
        void stergeBuffer(); // Sterge doar imaginea din RAM, nu ecranul fizic.

        // METODA 07: stergeRand - Goleste in RAM numai cei 128 de octeti ai randului primit, numerotat 0..3, pastrand ceilalti trei randuri. Nu transmite pe OLED; pentru un rand invalid memoreaza ParametruInvalid si iese.
        void stergeRand(uint8_t rand); // Sterge doar randul 0, 1, 2 sau 3 din RAM.
        
        // METODA 08: deseneazaPixel - Primeste coordonatele x/y si starea aprins, gaseste octetul si bitul pixelului, apoi aprinde sau stinge numai acel bit in RAM. Ignora coordonatele din afara panoului si nu transmite pe I2C.
        void deseneazaPixel(int x, int y, bool aprins = true); // Modifica un pixel in buffer, fara transmisie I2C.

        // METODA 09: scrieCaracter - Primeste un caracter si pozitia lui, transforma literele mici in mari si deseneaza cinci coloane de font plus una libera. Inlocuieste si pixelii stinsi ai celulei de 6x8, fara actualizare fizica.
        void scrieCaracter(int x, int y, char caracter); // Deseneaza un singur caracter si spatiul lui liber.

        // METODA 10: scrieText - Parcurge sirul primit si apeleaza scrieCaracter pentru fiecare caracter, avansand cu sase pixeli. Se opreste la terminator sau marginea dreapta, ignora pointerul nul/originea invalida si modifica doar RAM-ul.
        void scrieText(int x, int y, const char* text); // Deseneaza textul in RAM, caracter cu caracter.




        // METODA: afiseaza - Verifica initializarea, selecteaza toate coloanele/paginile si trimite bufferul de 513 B: un octet de control plus 512 B de imagine. Returneaza succesul I2C si nu sterge imaginea dupa transmitere.
        bool afiseaza(); // Trimite imaginea din RAM; nu o sterge dupa transmitere.

        // METODA: setPornit - Trimite comanda ON daca pornit=true sau OFF daca este false, dupa verificarea initializarii. Nu intrerupe alimentarea fizica si nu sterge bufferul imaginii; returneaza succesul transferului.
        bool setPornit(bool pornit); // Aprinde/stinge panoul fara sa stearga imaginea salvata in buffer.









        bool refreshPanel();  // Retrimite configuratia OLED si imaginea pastrata in memoria STM32.

        //METODA  - isReady: definitia este in src/Display.cpp, la acelasi numar
        bool isReady() const;  //returnare initializarea/ultimul transfer a reusit, fara o verificare electronica

        //METODA  - getLastError: definitia este in src/Display.cpp, la acelasi numar
        EroareDisplay getLastError() const;  //returnareza printr-o valoare eroarea din lista EroareDisplay, fara pointer sau text. un OLED defect nu poate avertiza singur

        
        
        //METODA  - setBrightness: definitia este in src/Display.cpp, la acelasi numar
        bool setBrightness(uint8_t level);   //primeste 0=minim, 1=mediu, 2=maxim. apelantul decide cand si de ce schimba nivelul

        //METODA  - clear: definitia este in src/Display.cpp, la acelasi numar
        void clear();    //sterge numai memoria de pixeli din RAM; 

        //METODA - drawPixel: definitia este in src/Display.cpp, la acelasi numar
        void drawPixel(int x, int y, bool on = true);   //aprinde/stinge un pixel valid din buffer; implicit on=true

        //METODA - update: definitia este in src/Display.cpp, la acelasi numar
        bool update();   //trimite paginile modificate catre OLED; false inseamna ca OLED-ul nu a raspuns sau a esuat transferul
        
        //METODA - drawText: definitia este in src/Display.cpp, la acelasi numar
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
        bool initializat = false; // Inlocuieste numele ready; indica pregatirea comunicatiei, nu daca exista deja text pe ecran.
        bool limbaEngleza = false; // False inseamna texte in romana, true inseamna engleza.
        EroareDisplay ultimaEroare = EroareDisplay::Neinitializat; // Retine o valoare din lista de erori, fara pointer la text.



       
        uint8_t dirtyPages = 0;                                         //Ceii patru biti inferiori indica paginile de cate 8 linii care trebuie retransmise
        uint8_t contrast = 0x01;                                        // Porneste la contrast minim si retine apoi ultima valoare trimisa cu succes.
        bool ready = false;                                             //confirmare de comunicatie reusita, inclusiv dupa transmiterea secventei de configurare
        EroareDisplay lastError = EroareDisplay::Neinitializat;         //ultima eroare de comunicatie sau de parametrii primiti de metodele de afisare
        bool sendCommands(const uint8_t* commands, uint8_t length);     //trimite comenzi SSD1306 si returneaza succesul transferului (cel mult 31 de comenzi/parametrii SSD1306 cu prefixul de control 0x00)
        
        uint8_t buffer[512] = {};                                       //buffer local de pixeli, 128x32/8=512 octeti; un singur bit pentru fiecare pixel, initial toti pixelii sunt stinsi
        uint8_t fontColumn(char character, uint8_t column) const;       // Returneaza bitii unei coloane de caracter; nu acceseaza hardware.
        

        // Adresele si comenzile au nume proprii, ca sa nu fie ascunse in functii.

        static const uint8_t CONTROL_PIXELI = 0x40; // Prefix SSD1306: octetii urmatori sunt pixeli.
        static const uint8_t ULTIMA_PAGINA = 3; // A patra si ultima pagina de opt linii.

        static const uint16_t LATIME = 128; // Numarul coloanelor de pixeli.
        static const uint8_t INALTIME = 32; // Numarul liniilor de pixeli.

        static const uint16_t OCTETI_IMAGINE = 512; // Memoria imaginii: 128*32/8.

        static const uint32_t STABILIZARE_OLED_MS = 100; // Pauza de pornire pentru stabilizarea OLED-ului.


        static const uint8_t mastiPixeli[8]; // Valorile 1,2,4,...128, pentru cei opt biti ai unui octet.

        // METODA: setEroare - Primeste o valoare EroareDisplay si o memoreaza in ultimaEroare, pentru consultarea prin getLastError. Nu afiseaza mesaje si nu efectueaza recuperare automata.
        void setEroare(EroareDisplay eroare); // Memoreaza simplu eroarea.

        // METODA: initializarePini - Activeaza ceasurile GPIOB/AFIO, pastreaza I2C1 neremapat si configureaza PB6/PB7 ca alternate function open-drain prin HAL. Nu configureaza pinii componentelor colegilor si nu returneaza o valoare.
        void initializarePini(); // Configureaza numai PB6 si PB7 pentru I2C1.

        // METODA: initializareI2C - Activeaza I2C1, completeaza handle-ul HAL pentru 100 kHz si adrese pe sapte biti, apoi aplica HAL_I2C_Init. Returneaza true la succes sau false cu InitializareI2CEsuata memorata; nu detecteaza OLED-ul.
        bool initializareI2C(); // Completeaza handle-ul si apeleaza HAL_I2C_Init.

        // METODA: cautaOLED - Incearca adresele numite 0x3C si 0x3D, transformate in format HAL, si pastreaza adresa care raspunde. Returneaza true la primul ACK; altfel memoreaza DisplayFaraRaspuns si returneaza false.
        bool cautaOLED(); // Incearca cele doua adrese numite mai sus.

        // METODA: configureazaOLED - Construieste pachetul de comenzi numite pentru SSD1306 128x32: orientare, adresare, contrast initial si parametrii panoului. Il trimite prin transmite si returneaza rezultatul; nu porneste aici logica seifului.
        bool configureazaOLED(); // Trimite secventa SSD1306 pentru 128x32.

        // METODA: coloanaCaracter - Primeste un caracter pregatit si o coloana interna 0..4 si returneaza masca celor opt pozitii verticale din font. Cauta litere/cifre/semne si returneaza zero pentru spatiu sau simbol nesuportat; nu deseneaza singura.
        uint8_t coloanaCaracter(char caracter, uint8_t coloana) const; // Citeste cinci coloane de font, fara hardware.

        // METODA: deseneazaColoana - Primeste pozitia si un octet de font, verifica pe rand cei opt biti si apeleaza deseneazaPixel cu starea aprins/stins. Modifica o singura coloana in RAM, fara transmisie I2C.
        void deseneazaColoana(int x, int y, uint8_t valoare); // Deseneaza cei sapte pixeli ai unei coloane de caracter.

       
       
       
        // METODA  - drawBattery: definitia este in src/Display.cpp, la acelasi numar.
        void drawBattery(bool valid, uint8_t percent); // Deseneaza pictograma din datele primite; procent invalid inseamna semnul intrebarii.

        // METODA  - drawMaskedPin: definitia este in src/Display.cpp, la acelasi numar.
        void drawMaskedPin(uint8_t digitCount, bool cursorVisible); // Deseneaza asteriscuri si cursor din lungime, fara sa primeasca PIN-ul real.
        
        const char* text(const char* romanian, const char* english) const; // Alege una dintre doua etichete constante dupa preferinta vizuala.


};

#endif