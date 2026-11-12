#include <Cititor_RFID.h>
#include <stdint.h>






int main(void) {

    // Initializam infrastructura HAL mai intai de toate;
    HAL_Init();

    // Initializare cititor MFRC522;
    Cititor_RFID cititor_mfrc522;

    // Permite stabilizarea cititorului dupa alimentare, intre 100 si 200ms;
    HAL_Delay(100);

    // Citim versiunea MFRC522;
    uint8_t versiuneRFID = cititor_mfrc522.citesteVersiune();
    // afisare pe USART1;
    
/*
    // Bucla Principala!
    while (true)
    {
       
    }
    
*/

}

