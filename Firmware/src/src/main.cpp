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

int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

    Cititor_RFID cititor_mfrc522;

    while (true)
    {
        /* code */
    }
    

}
