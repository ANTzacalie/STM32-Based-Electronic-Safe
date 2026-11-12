
#include "../include/Cititor_RFID.h"




// Se executa utomat la creearea unui obiect de tip Cititor_RFID
Cititor_RFID::Cititor_RFID() 
{

    initializarePini();

}

/*
 * Configureaza pinii GPIO ai modulului MFRC522:
   - PA4 - NSS (Slave Select), activ LOW.
   - PB5 - RESET, activ LOW.
 */
void Cititor_RFID::initializarePini()
{

    

}

/*
    - Va memora un nou card;
*/
uint8_t Cititor_RFID::stocareCard()
{
    // TODO: Initializare SPI si MFRC522.
}

/*
 - Va detecta un card, va autentifica sectorul
 folosind Crypto1 si va compara credentialul citit cu cel salvat in memoria microcontrollerului.
 */
uint8_t Cititor_RFID::verificaCard()
{
   // TODO
    return 0;
}

// Seteaza eroarea;
void Cititor_RFID::setEroare(EroareRFID eroare)
{
    ultimaEroare = eroare;
}

// TODO
EroareRFID Cititor_RFID::handleEroare() 
{

    switch (ultimaEroare)
    {
        case EroareRFID::NicioEroare:
            // Nu este necesara nicio actiune.
            break;

        case EroareRFID::EroareSPI:
        case EroareRFID::Timeout:
            // TODO: Recuperare comunicatie SPI.
            break;

        case EroareRFID::AutentificareEsuata:
        case EroareRFID::CardNeautorizat:
            // Accesul ramane blocat.
            break;

        default:
            // TODO: Gestionarea celorlalte erori.
            break;
    }

    return ultimaEroare;

}