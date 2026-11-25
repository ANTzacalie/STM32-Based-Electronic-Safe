#ifndef MEMORIE_RFID_H
#define MEMORIE_RFID_H

#include <memory_handleError.h>


enum class RFID_MEM_ERR : uint8_t {

    NicioEroare = 0,

    // Erori privind parametrii functiilor;
    ParametruInvalid,
    PointerInvalid,
    LungimeInvalida,
    AdresaInvalida,
    AdresaNealiniata,
    MemorieInsuficienta,

    // Erori privind controlerul Flash;
    DeblocareEsuata,
    BlocareEsuata,
    FlashOcupat,
    FlashProtejat,
    Timeout,

    // Erori privind operatiile asupra memoriei;
    PaginaGoala,
    PaginaNegolita,
    StergereEsuata,
    ScriereEsuata,
    CitireEsuata,
    VerificareStergereEsuata,
    VerificareScriereEsuata,

    // Erori privind datele RFID stocate;
    DateInexistente,
    DateInvalide,
    DateCorupte,
    SemnaturaInvalida,
    VersiuneIncompatibila,
    CRCInvalid,

    // Erori de uz in testari/impl incomp.;
    OperatieNeimplementata,
    EroareNecunoscuta

};

/*
    #Scrie si citeste din memorie informatiile necesare auth MIFARE Classic 1K;
    #Se ocupa de toate operatiile pe memoria R/W in legatura cu auth cu MFRC522;

    - Pentru APM32F103CBT6 / STM32F103CBT6 conform datasheet, avem 128 KB Flash,
    organizați în pagini de câte 1 KB(1024 octeti), in toal 128 pagini.

    - Spatiul ocupat de firmware - spatiu rezervat in linker este 0x08000000(pag.1-ADR_INCEPUT) – 0x0801FBFF(pag.126-ADR_SFARSIT);

    - In configuratia actuala([si alte date vor fi stocate, asa ca paginile vor arata diferit in final]), din acest spatiu voi lua o
    pagina(o vom rezerva), mai exact ultima 0x0801FC00(ADR_INCEPUT) - 0x0801FFFF(ADR_SFARSIT);

    - 1KB fiind marimea intreaga a sectoarelor MIFARE Classic 1K, deci avem spatiul necesar pentru a realiza orice
    operatie de scriere/citire/auth din memorie cu MFRC522 pe MIFARE Classic 1K;

    #Adresa paginii[ADR_INCEPUT] = 0x08000000  + (Numar pagina x 1024);
    #Adresa paginii[ADR_SFARSIT] = ADR_INCEPUT + 1KB;

    ### Memoria Flash funcționează diferit față de RAM:
        - Prin programare putem transforma biții din 1 în 0.
        - Prin ștergere putem readuce biții din 0 în 1.

*/
class Memorie_RFID
{

    public:

        Memorie_RFID();

        RFID_MEM_ERR getUltimaEroare() const {
            return ultimaEroare;
        }

        /*

            - Verifica daca pagina flash rezervata este complet stearsa(contine doar 0xFF);
                => 1 - Pagina este goala;
                => 0 - exista date in pagina;

        */ 
        uint8_t paginaEsteGoala() const; // membrii nu pot fi modificati, read only;

        // se foloseste de HAL_FLASHEx_Erase pentru a sterge pagina 128(daca luam de la 0, pag 127);
        uint8_t stergePagina128();

    private:

        // Gestionarea erorilor HAL Flash;
        memory_handleError memoryError;

        // erori in lucrul cu memoria pentru cititorul RFID;
        RFID_MEM_ERR ultimaEroare = RFID_MEM_ERR::NicioEroare;

        // Inregistreaza o eroare noua, folosita doar de clasa intern;
        void setEroare(RFID_MEM_ERR eroare);

        // constexpr spune compilatorului sa seteaza valoarea in variabla la compilare, nu la runtime;
        // Adresa primei locatii din pagina 127;
        static constexpr uint32_t ADRESA_FLASH = 0x0801FC00U;

        // Dimensiunea paginii flash este de 1KB;
        static constexpr uint32_t DIMENSIUNE_PAGINA = 1024U;

    protected:
    
};

#endif