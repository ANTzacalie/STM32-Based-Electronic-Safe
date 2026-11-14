#include <Cititor_RFID.h>
#include "stm32f1xx_hal.h"
#include <stdint.h>

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

//////////////////////////////////// MFRC522_RFID ////////////////////////////////////
void MFRC522_RFID_INIT() {

    // Initializare clasa cititor;
    Cititor_RFID cititor_mfrc522;

    // Asteptam stabilizarea cititorului RFID.
    HAL_Delay(100);

    
    // ############ TEST ############# //

    // Citim registrul VersionReg al MFRC522, volatile type utilizat pentru a vedea in debugger;
    volatile uint8_t versiuneRFID = cititor_mfrc522.citesteVersiune(); // 0x92 | 0x91 | 0x82 | 0x90 | 0x88 | 0x89 | 0xB2 | 0x12[CIP-FALS]

    // Executam testul SPI o singura data;
    volatile uint8_t rezultatSPI = cititor_mfrc522.testComunicareSPI(); 

    // Inspectam valoarea de eroare la fiecare pas asa, mult mai usor;
    volatile EroareRFID err = cititor_mfrc522.getUltimaEroare();

    // TX_CONTROL trebuie sa intoarca 131 in decimal, care inseamna 10000011 in binar, prin asta confirmam ca biti de activare a antenei MFRC522 sunt setati 1 si ca antena este pornita;
    volatile uint8_t txControl = cititor_mfrc522.citesteRegistruTest(0x14);

    // intoarce true daca txControl este 131 in decimal;
    volatile bool antenaActivata = ((txControl & 0x03) == 0x03);
    
    // ############ TEST ############# //

}
//////////////////////////////////// MFRC522_RFID ////////////////////////////////////


int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

    MFRC522_RFID_INIT();


    // Bucla principala.
    while (true)
    {
        
        // MAIN LOOP
 
    }

}
