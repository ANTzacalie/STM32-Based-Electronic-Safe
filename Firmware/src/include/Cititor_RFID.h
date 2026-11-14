
#ifndef CITITOR_RFID_H
#define CITITOR_RFID_H

// libraria HAL pentru familia STM32F100+ 
#include "stm32f1xx_hal.h"
#include <stdint.h>

/*
 - O clasa enumerare pentru identificarea erorilor RFID.
 - Fiecare valoare reprezinta o situatie care poate aparea in timpul functionarii cititorului.
*/

enum class EroareRFID : uint8_t
{
    NicioEroare = 0,       // RFID: Functionare normala.
    InitializareEsuata,    // RFID: MFRC522 nu a fost initializat.
    EroareSPI,             // RFID: Comunicare SPI esuata.
    Timeout,               // RFID: Cititorul nu raspunde la timp.
    CardAbsent,            // RFID: Nu exista card detectat.
    AutentificareEsuata,   // RFID: Autentificarea Crypto1 a esuat.
    CardNeautorizat,       // RFID: Cardul nu este cel inregistrat.
    EroareCitire,          // RFID: Citirea cardului a esuat.
    EroareScriere,         // RFID: Scrierea cardului a esuat.
    EroareFlash,           // RFID: Eroare la salvarea in memoria MCU.
    OperatieNeimplementata // RFID: NU EXISTA OPERATIA.
}; // OBS: HAL_OK nu inseamna ca transferul SPI a fost executat, nu garanteaza ca MFRC522 a raspuns corect, de aceea mereu verificam rezultatul cat putem de bine;

/*
  - Clasa Cititor_RFID
 
  - Gestioneaza comunicarea cu modulul MFRC522
  si autentificarea cardurilor MIFARE Classic 1K.

  Ce trebuie sa faca in final:
  - Verifica daca un card este autorizat. 
    Returneaza:
     1 - Card autorizat.
     0 - Card neautorizat sau eroare.
  - Adauga un nou card, inlocuindul pe cel vechi in memeorie;
    
*/

class Cititor_RFID {

    public:

        // Initializeaza modul RFID,
        /*
          - Va configura porturile(perifericele) necesare MFRC522;
          - Va configura comunicatia SPI cu MFRC522;
          - si registrele necesare functionarii cititorului;
        */
        Cititor_RFID();

        uint8_t stocareCard();
        uint8_t verificaCard();

        // Returneaza ultima eroare inregistrata
        EroareRFID getUltimaEroare() const 
        {
          return ultimaEroare;
        }

        // Gestioneaza ultima eroare aparuta, returnand tipul erorii;
        EroareRFID handleEroare();

        // returneaza versiunea hardware a MFRC522 gasita, returneaza zero daca SPI esueaza;
        uint8_t citesteVersiune(); 

        // testeaza comunicatia SPI prin scriere si citire.
        uint8_t testComunicareSPI();

        // citeste registrul specifica, *doar pt teste;
        uint8_t citesteRegistruTest(uint8_t reg);

        /*
         - Mai multe informatii in .cpp;
         - Trimite REQA si asteapta ATQA.
         - atqa - buffer pentru cei doi octeti receptionati.
         - Returneaza:
           -> 1 - Card detectat si raspuns ATQA valid.
           -> 0 - Card absent sau eroare.
        */
        uint8_t detecteazaCard(uint8_t atqa[2]);

    private:

        // Initializeaza circuitul RF de 13,56 Mhz, mai multe in .cpp;
        bool activareAntena();        
        /*
          - Citeste un registru intern al MFRC522 prin SPI
          Are doi paramatrii:
            1. Adresa registrului;
            2. Variabila in care salvam rezultatul;
        */        
        bool citireRegistru(uint8_t reg, uint8_t &val);
        
        // initializare pini prin HAl;
        void initializarePini();

        // Structura speciala definita in HAL, care contine info necesare administrari SPI;
        // Handle HAL pentru perifericul SPI si starea comunicatiei cu modulul MRFC522;
        SPI_HandleTypeDef hspi1 = {};

        // initializare spi1 prin HAL;
        void initializareSPI();

        // instanta eroare cu ultima eroare = 0 (default)
        EroareRFID ultimaEroare = EroareRFID::NicioEroare;

        // Inregistreaza o eroare noua, folosita doar de clasa intern;
        void setEroare(EroareRFID eroare);

        // Scrie un octet intr-un registru MFRC522.
        bool scriereRegistru(uint8_t registru, uint8_t valoare);

    protected:


};

#endif // CITITOR_RFID_H
