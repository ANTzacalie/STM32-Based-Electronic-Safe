
#ifndef CITITOR_RFID_H
#define CITITOR_RFID_H

#include <stdint.h>

/*
 - O clasa enumerare pentru identificarea erorilor RFID.
 - Fiecare valoare reprezinta o situatie care poate aparea in timpul functionarii cititorului.
*/
enum class EroareRFID : uint8_t
{
    NicioEroare = 0,       // Functionare normala.
    InitializareEsuata,    // MFRC522 nu a fost initializat.
    EroareSPI,             // Comunicare SPI esuata.
    Timeout,               // Cititorul nu raspunde la timp.
    CardAbsent,            // Nu exista card detectat.
    AutentificareEsuata,   // Autentificarea Crypto1 a esuat.
    CardNeautorizat,       // Cardul nu este cel inregistrat.
    EroareCitire,          // Citirea cardului a esuat.
    EroareScriere,         // Scrierea cardului a esuat.
    EroareFlash            // Eroare la salvarea in memoria MCU.
};


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
          - Va configura comunicatia SPI cu MFRC522
          - si registrele necesare functionarii cititorului.
        */
        Cititor_RFID() {
          // de adaugat parametrii
        }

        uint8_t stocareCard();
        uint8_t verificaCard();

        // Returneaza ultima eroare inregistrata
        EroareRFID getUltimaEroare() const 
        {
          return ultimaEroare;
        }

        // Gestioneaza ultima eroare aparuta, returnand tipul erorii;
        EroareRFID handleEroare();

    private:
        
        EroareRFID ultimaEroare = EroareRFID::NicioEroare;

        // Inregistreaza o eroare noua, folosita doar de clasa intern;
        void setEroare(EroareRFID eroare);

    protected:


};

#endif // CITITOR_RFID_H
