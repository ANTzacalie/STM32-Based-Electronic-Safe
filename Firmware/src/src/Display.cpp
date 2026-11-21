
// Mario, include path la Display.h , il tot stergi, cum vede clasa unde implementezi metodele?
#include "Display.h"
#include <string.h>

Display::Display()
{
    //Trebuiesc initializate periferice dar nu inainte de HAL_Init
}

bool Display::begin()
{
    ready = false; //invalidare a vechii confirmari de comunicare, inclusiv la o reinitializare ceruta
    address = 0; //resetare adresa memorata pentru a repeta detectarea modulului
    __HAL_RCC_GPIOB_CLK_ENABLE();                           //Activare ceas GPIOB prin macro-ul HAL necesar configurarii PB6/PB7
    __HAL_RCC_I2C1_CLK_ENABLE();                            //Activeaza ceasul in I2C1 prin HAL
    GPIO_InitTypeDef pins = {};                             //Pregateste structura de configurare ceruta de HAL, cu toate campurile initial zero
    pins.Pin = GPIO_PIN_6 | GPIO_PIN_7;                     //Selecteaza impreuna PB6 (=SCL) si PB7 (=SDA)
    pins.Mode = GPIO_MODE_AF_OD;                            //Selecteaza functia alternativa open-drain folosita de I2C
    pins.Speed = GPIO_SPEED_FREQ_HIGH;                      //Stabileste viteza de ccomutare a pinilor; frecventa magistralei se seteaza separat.
    HAL_GPIO_Init(GPIOB, &pins);                            //Aplica configuratia prin HAL, trmitand adresa structurii pins   
    i2c.Instance = I2C1;                                    //Leaga handle-lul clasei de perifericul I2C1; nu acceseaza manual registrii acestuia
    i2c.Init.ClockSpeed = 100000;                           //Frecventa I2C initiata la 100 kHz
    i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;                   //Completeaza configuratia duty-cycle ceruta de HAL
    i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;      //Foloseste adrese I2C pe 7 biti pentru dispozitive
    i2c.Init.OwnAddress1 = 0;                               //Nu foloseste adresa proprie a microcontrolerului deorece STM32 este masterul comunicatiei
    //i2c.Init.DualAddressMode = I2C_DUALADDRESSDISABLE;      //Dezactiveaza a doua adresa proprie, nefolosita de OLED
    i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;      //Dezactiveaza raspunsul perifericului la general-call.
    i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;          //Dezactiveaza modul de intindere a semnalului SCL, nefolosit de OLED


    if (HAL_I2C_Init(&i2c) != HAL_OK) //Verifica daca HAL a reusit initializarea magistralei
    { //Deschide tratarea erorii de initializare I2C
        lastError = EroareDisplay::InitializareI2CEsuata; //Retine codul erorii de configurare HAL, dupa modelul lui Antonio
        return false; //Opreste initializarea fara a presupune ca OLED-ul este disponibil
    } //Inchide tratarea esecului HAL_I2C_Init
    

    for (uint8_t addr = 0x3C; addr <= 0x3D; ++addr)     //Incearca cele doua adrese uzuale SSD1306 pe 7 biti
        {
            if (HAL_I2C_IsDeviceReady(&i2c,uint16_t (addr << 1), 3, 10) == HAL_OK) //Verifica daca OLED-ul raspunde la adresa curenta
            {
                address = addr << 1; //Salveaza adresa OLED in format HAL (8 biti)
                break; 
            }
        }


    if (address == 0) //Daca nu s-a gasit niciun OLED la adresele uzuale
        {
            lastError = EroareDisplay::DisplayFaraRaspuns; //Retine codul erorii de comunicatie
            return false; //Opreste initializarea fara a presupune ca OLED-ul este disponibil
        }
    
    if (address == 0) //Daca nu s-a gasit niciun OLED la adresele uzuale
        {
            lastError = EroareDisplay::DisplayFaraRaspuns; //Retine codul erorii de comunicatie
            return false; //Opreste initializarea fara a presupune ca OLED-ul este disponibil
        }
    
    const uint8_t commands[] = {        //Pregatire comenzilor doar pentru controlerul OLED
        0xAE,                           // Display OFF
        0xD5, 0x80,                     // Configureaza ceasul intern/divizorul display-ului
        0xA8, 0x1F,                     // Selecteaza inaltimea display-ului la 32 linii/multiplexarea la 32 de linii: valoarea transmisa este 32-1
        0xD3, 0x00,                     // Seteaza offset-ul display-ului la 0 (vertical offset)
        0x40,                           // Seteaza start line la 0
        0x8D, 0x14,                     // Activeaza charge pump-ul intern al display-ului/pompa de sarcina folosita de modulul SSD1306
        0x20, 0x00,                     // Seteaza modul de adresare orizontala a memoriei display-ului
        0xA1, 0xC8                    // Seteaza orientarea segmentelor si sensul scanarii COM
        
    };

    return 1;
}