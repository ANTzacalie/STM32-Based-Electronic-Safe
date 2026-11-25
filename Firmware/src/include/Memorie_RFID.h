#ifndef MEMORIE_RFID_H
#define MEMORIE_RFID_H

#include <Safe.h>

/*
    #Scrie si citeste din memorie informatiile necesare auth MIFARE Classic 1K;
    #Se ocupa de toate operatiile pe memoria R/W in legatura cu auth cu MFRC522;

    - Pentru APM32F103CBT6 / STM32F103CBT6 conform datasheet, avem 128 KB Flash, 
    organizați în pagini de câte 1 KB(1024 octeti), in toal 128 pagini.

    - Spatiul ocupat de firmware - spatiu rezervat in linker este 0x08000000(pag.1-ADR_INCEPUT) – 0x0801FBFF(pag.126-ADR_SFARSIT);

    - In configuratia actuala([si alte date vor fi stocate, asa ca paginile vor arata diferit in final]), din acest spatiu voi lua o 
    pagina(o vom rezerva), mai exact ultima 0x0801FC00(ADR_INCEPUT) - 0x0801FFFF(ADR_SFARSIT);

    - 1KB fiind marimea intreaga a sectoarelor MIFARE Classic 1K, deci avem spatiul necesar pentru a realiza orice 
    operatie de scriere din memorie cu MFRC522 pe MIFARE Classic 1K;

    #Adresa paginii[ADR_INCEPUT] = 0x08000000  + (Numar pagina x 1024);
    #Adresa paginii[ADR_SFARSIT] = ADR_INCEPUT + 1KB;

*/

class Memorie_RFID
{

    public:

        Memorie_RFID();

        /*

            - Verifica daca pagina flash rezervata este complet stearsa(contine doar 0xFF);
                => 1 - Pagina este goala;
                => 0 - exista date in pagina;

        */ 
        uint8_t paginaEsteGoala() const; // membrii nu pot fi modificati, read only;

    private:

        // constexpr spune compilatorului sa seteaza valoarea in variabla la compilare, nu la runtime;
        // Adresa primei locatii din pagina 127;
        static constexpr uint32_t ADRESA_FLASH = 0x0801FC00U;

        // Dimensiunea paginii flash este de 1KB;
        static constexpr uint32_t DIMENSIUNE_PAGINA = 1024U;

    protected:
    
};

#endif