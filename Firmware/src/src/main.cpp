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

/////////////////////// MFRC522 RFID ///////////////////////
void MFRC522_RFID_INIT() {

    // Initializare clasa cititor;
    Cititor_RFID cititor_mfrc522;

    // Asteptam stabilizarea cititorului RFID.
    HAL_Delay(100);
    
    // ############ TEST ############# //
    while (true)
    {

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

    // Bucla principala.
    while (true)
    {
        
        // MAIN LOOP
 
    }

}
