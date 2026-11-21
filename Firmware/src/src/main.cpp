#include <Cititor_RFID.h>
#include <Safe.h>
#include <Display.h>

#include "stm32f1xx_hal.h"
/*
 - Handler pentru intreruperea SysTick.
 - HAL_IncTick() incrementeaza contorul HAL
 - utilizat de HAL_Delay() si de timeout-uri.
 - daca nu lam declara, HAL_Delay poate sa nu se mai termine niciodata
 
 - extern "C" permite identificarea corecta a functiei de catre codul startup.
 */
extern "C" void SysTick_Handler(void)
{
    HAL_IncTick();
}

/////////////////////// MFRC522 RFID ///////////////////////
// TEST
void MFRC522_RFID_INIT()
{
    while (true)
    {
     
    // Constructorul initializeaza GPIO, SPI si antena.
    Cititor_RFID cititor_mfrc522;

    // Permitem stabilizarea modulului RFID.
    HAL_Delay(100);

    /*
     - Configuratia testului:
        Sectorul 1 - blocuri absolute 4, 5, 6, 7.
        Key A initiala este cheia implicita MIFARE.
    */
    const uint8_t sector = 1;

    const uint8_t cheieNoua[6] = {
        0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF
    };

    // Noua Key A pentru sectorul 1. Facuta veche pentru reverse
    const uint8_t cheieVeche[6] = {
        0xA7, 0x31, 0xE8,
        0x42, 0xB9, 0x6C
    };

    // Buffere pentru identificarea cardului.
    uint8_t atqa[2] = {0};
    uint8_t uidCL1[5] = {0};
    uint8_t sak = 0;

    // Variabile pentru inspectarea rezultatelor in debugger;
    volatile uint8_t cardDetectat = 0;
    volatile uint8_t uidCitit = 0;
    volatile uint8_t selectat = 0;
    volatile uint8_t cheieModificata = 0;
    volatile uint8_t autentificatCuCheiaNoua = 0;
    volatile uint8_t blocCitit = 0;

    volatile bool sesiuneResetata = false;
    volatile EroareRFID eroareModificare =EroareRFID::NicioEroare;

    /*
     - PAS 1 - Asteptam prezentarea unui card.
     
     - Nu executam modificarea cheii in aceasta bucla.
     - Bucla este folosita exclusiv pentru detectare.
    */
    while (cardDetectat == 0)
    {
        cardDetectat = cititor_mfrc522.detecteazaCard(atqa);

        if (cardDetectat == 0)
        {
            HAL_Delay(200);
        }
    }

    /*
     - PAS 2 - Citim UID-ul si selectam cardul;
    */
    uidCitit = cititor_mfrc522.citesteUID_CL1(uidCL1);

    if (uidCitit == 1)
    {
        selectat = cititor_mfrc522.selecteazaCard_CL1(uidCL1, sak);
    }

    /*
     - PAS 3 - Modificam Key A.
     - Acceptam numai MIFARE Classic 1K, cu UID complet la Cascade Level 1.
    */
    if (selectat == 1 && sak == 0x08 && uidCL1[0] != 0x88)
    {
        cheieModificata = cititor_mfrc522.modificaKeyA(sector, uidCL1, cheieVeche, cheieNoua);
    }

    // Salvam eroarea inainte de resetarea sesiunii;
    eroareModificare = cititor_mfrc522.getUltimaEroare();

    /*
     - PAS 4 - Inchidem sesiunea Crypto1 si resetam starea cardului.
    */
    sesiuneResetata = cititor_mfrc522.resetareSesiuneRFID();

    /*
     - PAS 5 - Verificam Key A noua. 
     - Nu modificam nimic daca scrierea a esuat.
     - Pentru o noua autentificare trebuie sa repetam REQA, ANTICOLLISION si SELECT.
    */
    uint8_t atqa_nou[2] = {0};
    uint8_t uid_nou[5]  = {0};
    uint8_t sak_nou = 0;

    uint8_t dateBloc[16] = {0};

    if (cheieModificata == 1 && sesiuneResetata)
    {
        HAL_Delay(100);

        uint8_t detectatDinNou = cititor_mfrc522.detecteazaCard(atqa_nou);

        if (detectatDinNou == 1 && cititor_mfrc522.citesteUID_CL1(uid_nou) == 1)
        {
            // Verificam ca este acelasi card;
            bool acelasiCard = true;

            for (uint8_t i = 0; i < 4; i++)
            {
                if (uid_nou[i] != uidCL1[i])
                {
                    acelasiCard = false;
                    break;
                }
            }

            if (acelasiCard && cititor_mfrc522.selecteazaCard_CL1(uid_nou, sak_nou) == 1 && sak_nou == 0x08)
            {
                //Autentificam sectorul 1 folosind noua Key A;
                autentificatCuCheiaNoua = cititor_mfrc522.autentificaBlocKeyA(4, cheieNoua, uid_nou);

                // Verificam accesul la blocul 4;
                if (autentificatCuCheiaNoua == 1)
                {
                    blocCitit = cititor_mfrc522.citesteBloc(4, dateBloc);
                }
            }
        }
    }

    // Salvam eroarea de la verificarea noii chei;
    volatile EroareRFID eroareVerificare = cititor_mfrc522.getUltimaEroare();

    // Inchidem sesiunea RFID dupa verificare;
    volatile bool sesiuneFinalizata = cititor_mfrc522.resetareSesiuneRFID();

    //verificam rezultatele complete;
    __NOP();
    HAL_Delay(100);
    }
    
} // TEST

/////////////////////// MFRC522 RFID ///////////////////////

int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

    MFRC522_RFID_INIT();

}
