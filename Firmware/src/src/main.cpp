#include <Cititor_RFID.h>
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

        volatile uint8_t uidCitit = 0;
        if(cardDetectat == 1) uidCitit = cititor_mfrc522.citesteUID_CL1(uidCL1);

        uint8_t dateSelect[7] = {
            0x93, 0x70,
            0x55, 0x88, 0xF0, 0x06, 0x2B
        };

        // Buffer pentru cei doi octeti CRC.
        uint8_t crc[2] = {0};

        // Calculam CRC_A prin coprocesorul MFRC522.
        volatile bool rezultatCRC = cititor_mfrc522.calculeazaCRC_A(dateSelect, 7, crc);

        // Salvam separat valorile pentru debugger;
        volatile uint8_t crcLow = crc[0];
        volatile uint8_t crcHigh = crc[1];

        // Testam SELECT numai daca ANTICOLLISION a reusit;
        uint8_t sak = 0;

        volatile uint8_t selectat = cititor_mfrc522.selecteazaCard_CL1(uidCL1, sak);

        // Salvam rezultatul pentru a vedea in debugger;
        volatile uint8_t sakDebug = sak;

        volatile EroareRFID eroareSelect = cititor_mfrc522.getUltimaEroare();

        // Cheia implicita MIFARE Classic.
        uint8_t keyA[6] = {
            0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF
        };

        volatile uint8_t autentificat = 0;

        if (selectat == 1)
        {
            // Autentificam blocul 4 din sectorul 1.
            autentificat = cititor_mfrc522.autentificaBlocKeyA(
                4, keyA, uidCL1
            );
        }

        volatile EroareRFID eroareAutentificare =
        cititor_mfrc522.getUltimaEroare();
        // breakpoint
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
