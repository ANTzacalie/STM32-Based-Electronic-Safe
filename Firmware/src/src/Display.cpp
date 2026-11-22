
// Mario, include path la Display.h , il tot stergi, cum vede clasa unde implementezi metodele?
#include "Display.h"
#include <string.h>

//Metoda 1 - Display: declarata in Display.h, la acelasi numar
Display::Display()
{
    //Trebuiesc initializate periferice dar nu inainte de HAL_Init
}

//Metoda 2 - begin: declarata in Display.h, la acelasi numar
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
    i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;     //Dezactiveaza raspunsul perifericului la general-call.
    i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;         //Dezactiveaza modul de intindere a semnalului SCL, nefolosit de OLED


    if (HAL_I2C_Init(&i2c) != HAL_OK)                     //Verifica daca HAL a reusit initializarea magistralei
    {                                                     
        lastError = EroareDisplay::InitializareI2CEsuata; //Retine codul erorii de configurare HAL, dupa modelul lui Antonio
        return false;                                     //Opreste initializarea fara a presupune ca OLED-ul este disponibil
    } 
    

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
            return false;                                  //Opreste initializarea fara a presupune ca OLED-ul este disponibil
        }
    

    if (address == 0)                                      //Daca nu s-a gasit niciun OLED la adresele uzuale
        {
            lastError = EroareDisplay::DisplayFaraRaspuns; //Retine codul erorii de comunicatie
            return false;                                  //Opreste initializarea fara a presupune ca OLED-ul este disponibil
        }
    

    const uint8_t commands[] = {    //Pregatire comenzilor doar pentru controlerul OLED
        0xAE,                       // Display OFF/stinge temporar display-ul pentru a nu afisa datele de initializare
        0xD5, 0x80,                 // Configureaza ceasul intern/divizorul display-ului
        0xA8, 0x1F,                 // Selecteaza inaltimea display-ului la 32 linii/multiplexarea la 32 de linii: valoarea transmisa este 32-1
        0xD3, 0x00,                 // Seteaza offset-ul display-ului la 0 (vertical offset)
        0x40,                       // Seteaza start line la 0
        0x8D, 0x14,                 // Activeaza charge pump-ul intern al display-ului/pompa de sarcina folosita de modulul SSD1306
        0x20, 0x00,                 // Seteaza modul de adresare orizontala a memoriei display-ului
        0xA1, 0xC8,                 // Seteaza orientarea segmentelor si sensul scanarii COM
        0xDA, 0x02,                 // Seteaza configuratia pinilor COM/ selecteaza configuratia COM pentru panoul 128x32
        0x81, 0x01,                 // Seteaza nivelul de contrast la valoarea minima (1)
        0xD9, 0xF1,                 // Seteaza modul de preincarcare la valoarea uzuala (0xF1)/configureaza intervalul de preincarcare al panoului
        0xDB, 0x40,                 // Configureaza nivelul de descarcare VCOMHa (al panoului la valoarea uzuala (0x40)), comportamentul se verifica pe modelul fizic
        0xA4,                       // Seteaza modul de afisare normal (nu se ignora continutul memoriei RAM)
        0xA6,                       // Seteaza modul de afisare normal (nu se inverseaza continutul memoriei RAM)
        0xAF                        // Display ON /porneste afisarea dupa configurarea parametrilor
    };


    if (!sendCommands(commands, uint8_t(sizeof commands))) //Trimite secventa de comenzi si verifica rezultatul transferului
    { 
        return false;       //Propaga eroarea I2C deja retinuta de sendCommands
    } 
    clear();                //Sterge bufferul local de pixeli, fara a transmite inca un cadru separat
    dirtyPages = 0x0F;      //Forteaza trimiterea tuturor celor patru pagini, chiar daca bufferul era deja gol
    ready = true;           //Permite update dupa transmiterea reusita a secventei de configurare
    return update();        //Trimite cadrul initial gol; false inseamna ca nici initializarea completa nu a reusit
}

//Metoda 3 - isReady: declarata in Display.h, la acelasi numar
bool Display::isReady() const       //Defineste citirea starii software a modulului, fara modificarea obiectului
{
    return ready;                   //Returneaza rezultatul initializarii/ultimelor transferuri, nu o masurare electrica noua
}

//metoda 4 - getLastError: declarata in Display.h, la acelasi numar
EroareDisplay Display::getLastError() const 
{                            //Defineste metoda declarata in .h; rezultatul este un cod din lista de erori
    return lastError;        //Copiaza ultima eroare catre apelant; nu returneaza un pointer si nu modifica obiectul
}

//metoda 5 - setEnglish: declarata in Display.h, la acelasi numar
void Display::setEnglish(bool enabled) //Defineste alegerea preferintei vizuale, fara meniu sau persistenta
{
    english = enabled;                 //retine true pentru engleza si false pentru romana; apelantul redeseneaza apoi ecranul dorit
}

//metoda 6 - setBrightness: declarata in Display.h, la acelasi numar
bool Display::setBrightness(uint8_t level) //Defineste transmiterea unei trepte
{
    if (level > 2) //Accepta numai 0=minim, 1=mediu si 2=maxim
    {
        lastError = EroareDisplay::LuminozitateInvalida; //Retine eroarea de nivel primit, fara comenzi fizice gresite
        return false;                                    //Nu modifica display-ul pentru un nivel invalid
    }
   

    if (!ready)       //Cere initializarea reusita inaintea modificarii contrastului
    {
        return false; //Pastreaza diagnosticul de initializare/comunicatie deja existent
    }


    const uint8_t levels[] = {0x01, 0x70, 0xFF};      //Defineste cele trei valori de contrast, fara reguli despre # sau standby
    const uint8_t commands[] = {0x81, levels[level]}; //Pregateste comanda SSD1306 de contrast si valoarea selectata
    

    if (!sendCommands(commands, uint8_t(sizeof commands))) //Trimite schimbarea contrastului si verifica HAL
    {
        return false;           //Propaga esecul catre apelant, care va decide recuperarea
    }
    lastError = EroareDisplay::NicioEroare; //Marcheaza comanda de contrast ca reusita
    return true;                           //Confirma transmiterea treptei; nu afirma un anumit
}

//metoda 7 - clear: declarata in Display.h, la acelasi numar
void Display::clear() //Defineste stergerea memoriei grafice locale
{
    for (uint16_t index = 0; index < sizeof buffer; ++index) //Viziteaza exact cei 512 octeti ai framebufferului
    {
        if (buffer[index] != 0)                         //Evita marcarea unei pagini daca octetul era deja gol
        {
            buffer[index] = 0;                          //Stinge cei opt pixeli reprezentati de acest octet
            dirtyPages |= uint8_t(1U << (index / 128)); //Marcheaza pagina 0..3 care contine octetul modificat
        }
    }
} //panoul fizic este actualizat abia prin update

//metoda 8 - drawPixel: declarata in Display.h, la acelasi numar
void Display::drawPixel(int x, int y, bool on)  //Defineste desenarea/stergerea unui singur pixel in RAM
{                                               //Deschide verificarea coordonatelor
    if (x < 0 || x >= 128 || y < 0 || y >= 32)  //Accepta numai coordonate valide pentru panoul 128x32, verifica inclusiv valorile negative inainte de accesarea tabloului
    {
        return;                                 //Ignora pixelii care nu pot fi desenati
    }
    //const uint16_t index = uint16_t(y / 8) * 128 + uint16_t(x); //Calculeaza indexul octetului din buffer care contine pixelul (x,y)
    const uint16_t index = uint16_t(x + (y / 8) * 128);         //Calculeaza coloana x din pagina verticala y/8
    const uint8_t bit = uint8_t(1U << (y % 8));                 //Calculeaza masca bitului corespunzator pixelului din octetul buffer[index]/ selectarea bitului vertical al pixelului in octetul paginii
    const uint8_t previous = buffer[index];                     //Retine octetul vechi pentru a detecta daca desenul chiar s-a modificat


    if (on)                             //Alege aprinderea sau stingerea ceruta de apelant
    {
        buffer[index] |= bit;           //Aprinde bitul selectat prin OR, pastrand ceilalti pixeli
    }
    else                                //Selecteaza stingerea daca on este false
    {
        buffer[index] &= uint8_t(~bit); //Stinge numai bitul selectat prin AND cu masca inversata
    }


    if (previous != buffer[index])            //Verifica daca operatia a schimbat octetul fata de valoarea anterioara
    {
        dirtyPages |= uint8_t(1U << (y / 8)); //Marcheaza pagina pixelului pentru urmatorul transfer I2C
    }
} 

