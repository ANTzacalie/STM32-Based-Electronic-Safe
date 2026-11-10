
#include "../include/Cititor_RFID.h"

/*
 - Va configura comunicatia SPI cu MFRC522
 - si registrele necesare functionarii cititorului.
 */
void Cititor_RFID::initialize()
{
    // TODO: Initializare SPI si MFRC522.
}

/*
 - Va detecta un card, va autentifica sectorul
 folosind Crypto1 si va compara credentialul citit cu cel salvat in memoria microcontrollerului.
 */
uint8_t Cititor_RFID::verificaCard()
{
   // ......
    return 0;
}
