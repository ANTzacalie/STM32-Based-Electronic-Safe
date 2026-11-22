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
    volatile uint8_t paginaGoala = memorieRFID.paginaEsteGoala();

    // Breakpoint;
    __NOP();

}

/////////////////////// DISPLAY OLED ///////////////////////

// In debugger poti vedea rezultatul testului in aceasta variabila.
volatile EroareDisplay eroareTest = EroareDisplay::Neinitializat;

void DISPLAY_OLED_INIT()
{
    // Constructorul pregateste obiectul. begin() initializeaza I2C si OLED-ul.
    Display display;

    if (display.begin() && display.setBrightness(1))
    {
        // Fiecare argument apare pe un rand separat al ecranului.
        display.showMessage("TEST OLED", "STM32F103", "I2C OK", "DISPLAY PORNIT");
    }

    eroareTest = display.getLastError();
}

/////////////////////// DISPLAY OLED ///////////////////////

int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

    Memorie_RFID memorieRFID;

    MEMORIE_RFID_TEST(memorieRFID);

    Cititor_RFID cititor_mfrc522;

    DISPLAY_OLED_INIT();

    while (true)
    {
        HAL_Delay(100);
    }
    

}
