#include <Cititor_RFID.h>
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


int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

//////////////////////////////////// MFRC522_RFID ////////////////////////////////////

    // Initializare clasa cititor;
    Cititor_RFID cititor_mfrc522;

    // Asteptam stabilizarea cititorului RFID.
    HAL_Delay(100);

    // Citim registrul VersionReg al MFRC522, volatile type utilizat pentru a vedea in debugger;
    volatile uint8_t versiuneRFID = cititor_mfrc522.citesteVersiune(); // 0x92 | 0x91 | 0x82 | 0x90 | 0x88 | 0x89 | 0xB2 | 0x12[CIP-FALS]

//////////////////////////////////// MFRC522_RFID ////////////////////////////////////


    // Bucla principala.
    while (true)
    {
        
        // MAIN LOOP
 
    }

}
