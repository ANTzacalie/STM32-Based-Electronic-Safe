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
void MFRC522_RFID_INIT() {

    // Initializare clasa cititor;
    Cititor_RFID cititor_mfrc522;

    // Asteptam stabilizarea cititorului RFID.
    HAL_Delay(100);
    
    // ############ TEST ############# //
    while (true)
    {

        uint8_t atqa[2] = {0};
        uint8_t uidCL1[5] = {0};

        // pas1. detectam cardul prin REQA 1 = raspuns valid, 0 = card absent/eroare;
        volatile uint8_t cardDetectat = cititor_mfrc522.detecteazaCard(atqa);

        
        if(cardDetectat == 1) cititor_mfrc522.citesteUID_CL1(uidCL1);

        // Testam SELECT numai daca ANTICOLLISION a reusit;
        uint8_t sak = 0;

        uint8_t selectat = cititor_mfrc522.selecteazaCard_CL1(uidCL1, sak);

        // Cheia implicita MIFARE Classic pe o cartele goala;;
        uint8_t keyA[6] = {
            0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF
        };

        volatile uint8_t autentificat = 0;

        if (selectat == 1)
        {
            // Autentificam blocul 4 din sectorul 1;
            autentificat = cititor_mfrc522.autentificaBlocKeyA(
                4, keyA, uidCL1
            );
        }

        // Buffer pentru continutul blocului 4.
        uint8_t dateBloc[16] = {0};

        // Retinem rezultatul operatiei READ, SECTORUL 0 de pe card, din cele 16;
        volatile uint8_t blocCitit = 0;

        // Citim blocul numai dupa autentificarea reusita;
        if (autentificat == 1)
        {
            blocCitit = cititor_mfrc522.citesteBloc(4, dateBloc);
        }

        volatile EroareRFID eroareAutentificare = cititor_mfrc522.getUltimaEroare();
        
        // Datele pe care dorim sa le scriem in blocul 4.
        uint8_t dateScriere[16] = {
            0x11, 0x22, 0xC5, 0x44,
            0x55, 0x66, 0x77, 0x88,
            0x99, 0xAA, 0xBB, 0xCC,
            0xDD, 0xEE, 0xFF, 0x10
        };

        // Buffer pentru verificarea datelor scrise.
        uint8_t dateVerificare[16] = {0};

        volatile uint8_t blocScris = 0;
        volatile uint8_t blocRecitit = 0;

        if (autentificat == 1)
        {
            // Scriem datele in memoria cardului.
            blocScris = cititor_mfrc522.scrieBloc(4, dateScriere);

            // Recitim blocul numai daca scrierea a reusit.
            if (blocScris == 1)
            {
                blocRecitit = cititor_mfrc522.citesteBloc(4, dateVerificare);
            }

        }

        // Salvam eroarea inainte de resetarea sesiunii.
        volatile EroareRFID eroareScriere = cititor_mfrc522.getUltimaEroare();
        cititor_mfrc522.resetareSesiuneRFID();

        // Breakpoint aici.
        __NOP();

        
        HAL_Delay(200);

    }

    // ############ TEST ############# //

}

/////////////////////// MFRC522 RFID ///////////////////////

int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

    MFRC522_RFID_INIT();

}
