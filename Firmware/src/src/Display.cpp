
// Mario, include path la Display.h , il tot stergi, cum vede clasa unde implementezi metodele?
#include "Display.h"
#include <string.h>

Display::Display()
{
    //Trebuiesc initializate periferice dar nu inainte de HAL_Init
}

bool Display::begin()
{
    ready_ = false; //invalidare a vechii confirmari de comunicare, inclusiv la o reinitializare ceruta
    address_ = 0; //resetare adresa memorata pentru a repeta detectarea modulului
    __HAL_RCC_GPIOB_CLK_ENABLE();                           //Activare ceas GPIOB prin macro-ul HAL necesar configurarii PB6/PB7
    __HAL_RCC_I2C1_CLK_ENABLE();                            //Activeaza ceasul in I2C1 prin HAL
    GPIO_InitTypeDef pins = {};                             //Pregateste structura de configurare ceruta de HAL, cu toate campurile initial zero
    pins.Pin = GPIO_PIN_6 | GPIO_PIN_7;                     //Selecteaza impreuna PB6 (=SCL) si PB7 (=SDA)
    pins.Mode = GPIO_MODE_AF_OD;                            //Selecteaza functia alternativa open-drain folosita de I2C
    pins.Speed = GPIO_SPEED_FREQ_HIGH;                      //Stabileste viteza de ccomutare a pinilor; frecventa magistralei se seteaza separat.
    HAL_GPIO_Init(GPIOB, &pins);                            //Aplica configuratia prin HAL, trmitand adresa structurii pins   
    i2c.Instance = I2C1;                                    //Leaga handle-lul clasei de perifericul I2C1; nu acceseaza manual registrii acestuia
    i2c.Init.ClockSpeed = 100000;                           //Frecventa I2C initiata la 100 kHz
    i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;                 //Completeaza configuratia duty-cycle ceruta de HAL
    i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;     //Foloseste adrese I2C pe 7 biti pentru dispozitive
   

    

    

    return 1;
}