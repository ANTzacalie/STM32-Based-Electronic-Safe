
#ifndef CITITOR_RFID_H
#define CITITOR_RFID_H

#include <stdint.h>

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

    private:

    protected:


};

#endif // CITITOR_RFID_H
