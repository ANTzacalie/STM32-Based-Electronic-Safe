#include <Cititor_RFID.h>
#include <Memorie_RFID.h>
#include <Safe.h>
#include <Display_SSD1306.h>

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

void MEMORIE_RFID_TEST(Memorie_RFID memorieRFID)
{

    // Verificam daca ultima pagina flash este goala;
    // volatile uint8_t paginaGoala = memorieRFID.paginaEsteGoala();

    /*
     - TEST: scrierea a 16 biti in pagina Flash 127;
    */
    const uint32_t adresaTest = 0x0801FC00U;
    const uint16_t valoareTest = 0x5A3C;

    // Citim valoarea existenta in Flash;
    volatile uint16_t valoareInainte = *reinterpret_cast<volatile const uint16_t *>(adresaTest);

    // pt debugger;
    volatile HAL_StatusTypeDef rezultatScriere = HAL_ERROR;
    volatile uint32_t eroareFlash = 0;

    /*
     - Scriem numai daca locatia este stearsa in Flash, starea stearsa este 0xFFFF;
    */
    if (valoareInainte == 0xFFFFU)
    {
        // Deblocam controlerul Flash pentru programare;
        if (HAL_FLASH_Unlock() == HAL_OK)
        {
            // Programam 16 biti la adresa aleasa;
            rezultatScriere = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, adresaTest, valoareTest);

            // Salvam codul erorii, daca programarea a esuat;
            if (rezultatScriere != HAL_OK)
            {
                eroareFlash = HAL_FLASH_GetError();
            }

            // Blocam din nou operatiile de scriere Flash;
            HAL_FLASH_Lock();
        }
    }

    // Recitim locatia dupa tentativa de programare;
    volatile uint16_t valoareDupa = *reinterpret_cast<volatile const uint16_t *>(adresaTest);

    // Breakpoint;
    __NOP();
}

int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

    Memorie_RFID memorieRFID;
    Cititor_RFID cititor_mfrc522;

    while (true)
    {
        MEMORIE_RFID_TEST(memorieRFID);
        HAL_Delay(100);
    }

}
