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

volatile EroareDisplay eroareTest = EroareDisplay::Neinitializat; // Rezultatul vizibil in debugger.
static Display display;          // Acelasi obiect si acelasi buffer sunt folosite de ambele functii.
static uint32_t ultimaIncercare; // Momentul ultimei incercari, in milisecunde de la pornire.

void DISPLAY_OLED_INIT()         // Se apeleaza la pornire si dupa o pierdere de comunicatie.
{
    ultimaIncercare = HAL_GetTick();  // Retine momentul la care incepe incercarea.

    if (display.begin() && display.setBrightness(1)) // Detecteaza OLED-ul, apoi seteaza contrastul mediu.
    {
        display.showMessage("TEST OLED", "STM32F103", "I2C OK", "DISPLAY PORNIT"); // Afiseaza 4 randuri.
    }

    eroareTest = display.getLastError(); // Salveaza rezultatul ultimei operatii pentru debugger.
}

void DISPLAY_OLED_SERVICE()      // Se apeleaza repetat din bucla principala a seifului.
{
    if (HAL_GetTick() - ultimaIncercare < 1000U) // Verifica daca a trecut o secunda de la ultima incercare.
    {
        return;                  // Este prea devreme. Programul continua imediat cu restul seifului.
    }

    ultimaIncercare = HAL_GetTick(); // Memoreaza momentul verificarii curente.

    if (!display.isReady())      // Consulta starea memorata de driver dupa ultima comunicatie.
    {
        DISPLAY_OLED_INIT();     // Reincearca detectarea adresei si initializarea OLED-ului.
        return;                  // Initializarea tocmai a incercat sa trimita si imaginea.
    }

    display.refreshPanel();               // Reaplica setarile si imaginea daca OLED-ul a repornit separat.
    eroareTest = display.getLastError();  // Pastreaza rezultatul. La esec, driverul va cere reinitializare.
}

/////////////////////// DISPLAY OLED ///////////////////////

int main(void)
{
    // Initializam infrastructura HAL.
    HAL_Init();

    Memorie_RFID memorieRFID;
    Cititor_RFID cititor_mfrc522;

    MEMORIE_RFID_TEST(memorieRFID);


    DISPLAY_OLED_INIT();

    while (true)
    {
        DISPLAY_OLED_SERVICE();    // Verifica daca a trecut o secunda si comunica cu OLED-ul.
        HAL_Delay(100);
    }
    

}
