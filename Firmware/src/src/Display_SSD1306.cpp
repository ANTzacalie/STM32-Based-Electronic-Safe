
// Mario, include path la Display.h , il tot stergi, cum vede clasa unde implementezi metodele?
#include "Display_SSD1306.h"
#include <string.h>
#include <stdio.h> // Include snprintf pentru formatarea textelor cu limita de dimensiune.

// TABELELE FONTULUI: definite o singura data, dupa declaratiile lor din clasa.
// Font monocrom 5x7: un grup are cinci coloane, iar bitii 0..6 ai fiecarei coloane indica pixelii de sus in jos.
const uint8_t letters[26][5] = // Defineste tabelul constant al literelor, comun tuturor instantelor clasei.
{ 
    {126, 9, 9, 9, 126},        // Defineste cele cinci coloane ale literei A.
    {127, 73, 73, 73, 54},      // Defineste litera B; numerele sunt masti de pixeli, nu adrese de registre.
    {62, 65, 65, 65, 34},       // Defineste litera C.
    {127, 65, 65, 34, 28},      // Defineste litera D.
    {127, 73, 73, 73, 65},      // Defineste litera E.
    {127, 9, 9, 9, 1},          // Defineste litera F.
    {62, 65, 73, 73, 122},      // Defineste litera G.
    {127, 8, 8, 8, 127},        // Defineste litera H.
    {0, 65, 127, 65, 0},        // Defineste litera I.
    {32, 64, 65, 63, 1},        // Defineste litera J.
    {127, 8, 20, 34, 65},       // Defineste litera K.
    {127, 64, 64, 64, 64},      // Defineste litera L.
    {127, 2, 12, 2, 127},       // Defineste litera M.
    {127, 4, 8, 16, 127},       // Defineste litera N.
    {62, 65, 65, 65, 62},       // Defineste litera O.
    {127, 9, 9, 9, 6},          // Defineste litera P.
    {62, 65, 81, 33, 94},       // Defineste litera Q.
    {127, 9, 25, 41, 70},       // Defineste litera R.
    {38, 73, 73, 73, 50},       // Defineste litera S.
    {1, 1, 127, 1, 1},          // Defineste litera T.
    {63, 64, 64, 64, 63},       // Defineste litera U.
    {31, 32, 64, 32, 31},       // Defineste litera V.
    {63, 64, 56, 64, 63},       // Defineste litera W.
    {99, 20, 8, 20, 99},        // Defineste litera X.
    {7, 8, 112, 8, 7},          // Defineste litera Y.
    {97, 81, 73, 69, 67}        // Defineste litera Z, ultima intrare a tabelului.
}; 

static const uint8_t digits[10][5] = // Defineste cele zece cifre in aceeasi organizare de cinci coloane.
{ 
    {62, 81, 73, 69, 62},     // Defineste cifra 0.
    {0, 66, 127, 64, 0},      // Defineste cifra 1.
    {98, 81, 73, 73, 70},     // Defineste cifra 2.
    {34, 65, 73, 73, 54},     // Defineste cifra 3.
    {24, 20, 18, 127, 16},    // Defineste cifra 4.
    {39, 69, 69, 69, 57},     // Defineste cifra 5.
    {62, 73, 73, 73, 50},     // Defineste cifra 6.
    {1, 113, 9, 5, 3},        // Defineste cifra 7.
    {54, 73, 73, 73, 54},     // Defineste cifra 8.
    {38, 73, 73, 73, 62}      // Defineste cifra 9.
}; 




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
    i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;     //Dezactiveaza a doua adresa proprie, nefolosita de OLED
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
    

        ready = true;           // OLED-ul a raspuns la adresa I2C. Permitem trimiterea configuratiei.
    return refreshPanel();      // begin() returneaza true numai daca reuseste si trimiterea imaginii.
} // Aici se termina Display::begin().

// Reaplica setarile panoului si retrimite imaginea care exista deja in buffer.
bool Display::refreshPanel()
{
    if (!ready)             // Verifica daca driverul considera comunicatia disponibila.
    {
        return false;       // DISPLAY_OLED_SERVICE() va apela begin() pentru o noua detectare.
    }

    const uint8_t commands[] = {  // Comenzile de configurare pentru SSD1306 de 128x32.
        0xD5, 0x80,               // Configureaza ceasul intern al controlerului OLED.
        0xA8, 0x1F,               // Selecteaza 32 de linii de afisare: 0x1F inseamna 32 - 1.
        0xD3, 0x00,               // Stabileste deplasarea verticala la zero.
        0x40,                     // Afisarea incepe de la linia zero.
        0x8D, 0x14,               // Activeaza pompa interna de tensiune a OLED-ului.
        0x20, 0x00,               // Selecteaza adresarea orizontala a memoriei de pixeli.
        0xA1, 0xC8,               // Stabileste orientarea coloanelor si sensul de scanare a liniilor.
        0xDA, 0x02,               // Configureaza iesirile COM pentru panoul de 128x32.
        0x81, contrast,           // Reaplica ultima valoare de contrast memorata de clasa.
        0xD9, 0xF1,               // Configureaza intervalul de preincarcare a pixelilor.
        0xDB, 0x40,               // Configureaza nivelul electric VCOMH al panoului.
        0xA4,                     // Afiseaza pixelii conform continutului memoriei OLED.
        0xA6,                     // Selecteaza afisarea normala, cu pixelii neinversati.
        0xAF                      // Porneste afisarea.
    };

    if (!sendCommands(commands, uint8_t(sizeof commands)))  // Trimite tot tabloul prin I2C.
    {
        return false;         // sendCommands() a memorat deja eroarea de comunicatie.
    }

    dirtyPages = 0x0F;        // 0x0F = 00001111: marcheaza toate cele 4 pagini pentru trimitere.
    return update();          // Copiaza imaginea din memoria STM32 in memoria OLED-ului.
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

    contrast = levels[level];  // Memoreaza valoarea trimisa efectiv: 0x01, 0x70 sau 0xFF.

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

// METODA 09 - drawText: declarata in include/Display.h, la acelasi numar.
void Display::drawText(int x, int y, const char* value) // Defineste desenarea unui sir primit din exterior.
{ 
    if (value == nullptr || x < 0 || y < 0 || y >= 32)  // Respinge pointerul nul si originea din afara zonei verticale sau la stanga panoului.
    { 
        return;                                         // Nu citeste un pointer invalid si nu deseneaza in afara framebufferului.
    } 


    for (; *value != '\0' && x <= 123; ++value, x += 6) // Parcurge sirul pana la terminator sau ultima origine la care incape un caracter de cinci pixeli.
    { 
        for (uint8_t column = 0; column < 5; ++column)  // Parcurge cele cinci coloane ale caracterului.
        { 
            const uint8_t bits = fontColumn(*value, column); // Citeste masca de sapte pixeli verticali din font.
            for (uint8_t bit = 0; bit < 7; ++bit)            // Parcurge cei sapte biti verticali utili ai mastii.
            {                                                // Deschide verificarea unui pixel al literei.
                if ((bits & (1U << bit)) != 0)               // Selecteaza numai pixelii aprinsi din desenul caracterului.
                { 
                    drawPixel(x + column, y + bit);          // Foloseste verificarea limitelor si marcarea paginii din drawPixel.
                } 
            } 
        } 
    } 
} 

// METODA 10 - update: declarata in include/Display.h, la acelasi numar.
bool Display::update() // Defineste transferul paginilor modificate catre OLED.
{ 
    if (!ready)                              // Refuza transmiterea inainte de initializare sau dupa un esec de comunicatie.
    { 
        return false;                        // Pastreaza diagnosticul anterior si cere apelantului sa trateze problema.
    } 


    for (uint8_t page = 0; page < 4; ++page)          // Parcurge cele patru pagini de cate opt linii ale panoului 128x32.
    { 
        const uint8_t pageMask = uint8_t(1U << page); // Construieste bitul care identifica pagina curenta in dirtyPages_.
        if ((dirtyPages & pageMask) == 0)             // Verifica daca pagina nu a fost atinsa de desenare/stergere.
        {
            continue;                                 // Sare direct la pagina urmatoare, economisind trafic I2C.
        }
        
        const uint8_t range[] = {0x21, 0, 127, 0x22, page, page}; // Selecteaza coloanele 0..127 si numai pagina curenta, prin comenzi SSD1306.
        
        if (!sendCommands(range, uint8_t(sizeof range))) // Configureaza fereastra de transfer si verifica succesul ei.
        { 
            return false; // Nu trimite date fara o fereastra de adresare confirmata de transferul HAL.
        } 

        uint8_t packet[17] = {}; // Rezerva doar 17 B suplimentari pe stack: prefix plus 16 B de pixeli.
        packet[0] = 0x40; // Prefixul 0x40 indica date grafice SSD1306, nu comenzi.
        
        for (uint8_t offset = 0; offset < 128; offset = uint8_t(offset + 16)) // Parcurge pagina de 128 B in opt pachete de cate 16 B.
        { 
            memcpy(packet + 1, buffer + uint16_t(page) * 128 + offset, 16);   // Copiaza cei 16 B din pagina, dupa prefixul de control.
            if (HAL_I2C_Master_Transmit(&i2c, address, packet, sizeof packet, 10) != HAL_OK) // Transmite sincron pachetul, cu timeout finit de 10 ms.
            { 
                ready = false;                                      // Marcheaza comunicatia ca esuata pana la reinitializare.
                lastError = EroareDisplay::TransferPixeliEsuat;     // Retine esecul transmiterii pixelilor.
                return false;                                       // Opreste transferurile ramase; nu pretinde ca pagina a fost afisata complet.
            } 
        } 
        dirtyPages &= uint8_t(~pageMask);   // Sterge marcajul paginii numai dupa transmiterea ei completa cu succes.
    } 

    lastError = EroareDisplay::NicioEroare; // Marcheaza actualizarea reusita, inclusiv cazul fara pagini de transmis.
    return true;                            // Confirma finalizarea actualizarii; nu exprima starea usii, PIN-ului sau alarmei.

}

// METODA 11 - getBuffer: declarata in include/Display.h, la acelasi numar.
const uint8_t* Display::getBuffer() const // Defineste accesul la framebuffer pentru verificare vizuala si teste.
{ 
    return buffer; // Returneaza un pointer const: apelantul poate inspecta, dar nu modifica pixelii prin acest pointer.
} 


/*

   $$$ PROBLEMA DE COMUNICATIE ATUNCI CAND SURSA DE TENSIUNE ESTE INCHISA SI REDESCHISA, ECRANUL NU SE MAI APRINDE;

*/
// METODA 12 - showMessage: declarata in include/Display.h, la acelasi numar.
bool Display::showMessage(const char* title, const char* row2, const char* row3, const char* row4) // Defineste macheta comuna pentru patru randuri.
{ // Deschide afisarea mesajului.
    if (!ready) // Cere initializarea reusita inaintea construirii unui ecran de prezentat.
    { // Deschide cazul OLED-ului indisponibil.
        return false; // Raporteaza esecul fara promisiunea ca mesajul de eroare ar fi vizibil pe un OLED defect.
    } // Inchide verificarea disponibilitatii.
    clear(); // Sterge macheta precedenta din RAM, marcand paginile atinse.
    drawText(0, 0, title); // Deseneaza titlul pe randul 1, la y=0.
    drawText(0, 8, row2); // Deseneaza randul 2 la y=8; pointerul nul este ignorat in siguranta.
    drawText(0, 16, row3); // Deseneaza randul 3 la y=16.
    drawText(0, 24, row4); // Deseneaza randul 4 la y=24, fara margini care ar consuma o linie.
    return update(); // Transmite macheta si returneaza rezultatul transportului OLED.
} // Incheie showMessage.


// METODA 13 - showHome: declarata in include/Display.h, la acelasi numar.
bool Display::showHome(const char* status, const char* timeText, const char* dateText, bool batteryValid, uint8_t batteryPercent) // Defineste ecranul general din informatii deja pregatite de apelant.
{ 
    if (!ready)       // Verifica disponibilitatea software a OLED-ului.
    { 
        return false; // Lasa tratarea defectului la apelant, fara a confirma vreo stare a seifului.
    } 
    clear();          // Curata imaginea precedenta in RAM.
    char title[19] = {};   // Rezerva maximum 18 caractere plus terminator, lasand loc pictogramei bateriei pe primul rand.
    if (status != nullptr) // Verifica existenta textului de stare primit din exterior.
    { 
        snprintf(title, sizeof title, "%s", status); // Copiaza cel mult cat incape in titlu; textul mai lung este trunchiat sigur.
    } 
    drawText(0, 0, title);      // Deseneaza exact starea transmisa de logica viitoare, fara a o deduce din PIN/card.
    drawBattery(batteryValid, batteryPercent); // Deseneaza bateria pe baza datelor modulului de alimentare.
    drawText(0, 8, timeText);   // Afiseaza ora primita ca text; nu o calculeaza si nu o pastreaza ca RTC.
    drawText(0, 16, dateText);  // Afiseaza data primita ca text, fara validare calendaristica.
    drawText(0, 24, text("INTRODU COD / CARD", "ENTER PIN / USE CARD")); // Deseneaza instructiunea de acces in limba selectata.
    return update();            // Transmite ecranul si raporteaza succesul afisarii.
} 

// METODA 14 - showPin: declarata in include/Display.h, la acelasi numar.
bool Display::showPin(uint8_t digitCount, uint8_t attemptsLeft, bool cursorVisible) // Defineste macheta introducerii PIN-ului din trei valori primite.
{ 
    if (digitCount > 12 || attemptsLeft > 3) // Respinge valori imposibile pentru macheta convenita, fara sa implementeze regula autentificarii.
    { 
        lastError = EroareDisplay::DatePinInvalide; // Retine codul parametrilor vizuali PIN invalizi.
        return false; // Pastreaza vechea imagine si nu acceseaza bufferul cu un indice invalid.
    } 


    if (!ready)       // Cere initializarea reusita a OLED-ului.
    { 
        return false; // Raporteaza esecul fara sa consume o incercare PIN.
    } 


    clear();          // Sterge imaginea anterioara in RAM.
    drawText(0, 0, text("INTRODUCETI CODUL", "ENTER PIN")); // Deseneaza titlul introducerii PIN-ului.
    drawMaskedPin(digitCount, cursorVisible); // Deseneaza numai lungimea si faza cursorului furnizate de apelant.
    
    char attempts[22] = {}; // Rezerva un rand de maximum 21 caractere plus terminator.
    snprintf(attempts, sizeof attempts, "%s: %u", text("INCERCARI", "ATTEMPTS"), unsigned(attemptsLeft)); // Formateaza valoarea primita, fara decrementare sau resetare.
    drawText(0, 16, attempts); // Afiseaza incercarile pe al treilea rand.
    drawText(0, 24, text("*:STERGE #:LUMINA", "*:DELETE #:BRIGHT")); // Deseneaza doar etichetele tastelor; nu proceseaza apasarile lor.
    
    return update(); // Transmite macheta si returneaza rezultatul comunicatiei.
} 


// METODA 15 - showPinIncorrect: declarata in include/Display.h, la acelasi numar.
bool Display::showPinIncorrect(uint8_t attemptsLeft) // Defineste mesajul PIN incorect cu incercari furnizate de logica viitoare.
{ 
    if (attemptsLeft > 3) // Respinge un numar care nu corespunde machetei cu maximum trei incercari.
    { 
        lastError = EroareDisplay::IncercariInvalide; // Memoreaza eroarea valorii de incercari primite pentru afisare.
        return false; // Nu schimba imaginea si nu modifica vreun istoric de autentificare.
    } 
    char attempts[22] = {}; // Pregateste textul temporar al incercarilor, fara alocare dinamica.
    snprintf(attempts, sizeof attempts, "%s: %u", text("INCERCARI", "ATTEMPTS"), unsigned(attemptsLeft)); // Construieste eticheta si valoarea exact asa cum au fost furnizate.
    return showMessage(text("COD INCORECT", "WRONG PIN"), attempts, text("REINCERCATI", "TRY AGAIN"), ""); // Foloseste macheta comuna si propaga rezultatul transmisiei.
} 

// METODA 16 - showPinCorrect: declarata in include/Display.h, la acelasi numar.
bool Display::showPinCorrect() // Defineste ecranul de succes al autentificarii, furnizat ca rezultat din exterior.
{  
    return showMessage(text("COD CORECT", "PIN ACCEPTED"), "", "", ""); // Afiseaza succesul primit; nu acorda ea insasi acces si nu controleaza servo.
} 

// METODA 17 - showPinMismatch: declarata in include/Display.h, la acelasi numar.
bool Display::showPinMismatch() // Defineste eroarea vizuala a repetarii unui PIN nou.
{ 
    return showMessage(text("CODURILE DIFERA", "PIN DOES NOT MATCH"), text("REINCERCATI", "TRY AGAIN"), "", ""); // Prezinta un rezultat de comparare efectuat de logica viitoare, fara sa compare date secrete.
} 



uint8_t Display::fontColumn(char character, uint8_t column) const // Defineste obtinerea desenului unei singure coloane de caracter.
{ 
    if (column >= 5) // Protejeaza indicii celor cinci coloane disponibile.
    { 
        return 0; // Intoarce o coloana goala fara acces in afara tabelelor.
    } 

    if (character >= 'a' && character <= 'z') // Recunoaste literele mici ASCII.
    { 
        character = char(character - 'a' + 'A'); // Refoloseste fontul A..Z, fara un al doilea set de litere.
    } 

    if (character >= 'A' && character <= 'Z') // Selecteaza tabelul literelor pentru o majuscula valida.
    { 
        return letters[character - 'A'][column]; // Transforma ASCII in indice 0..25 si returneaza masca coloanei.
    } 

    if (character >= '0' && character <= '9') // Selecteaza tabelul cifrelor.
    { 
        return digits[character - '0'][column]; // Transforma ASCII in indice 0..9 si intoarce masca de pixeli.
    } 

    switch (character) // Alege semnele de punctuatie folosite de ecrane.
    { 
        
    case '*': // Selecteaza asteriscul folosit la mascarea PIN-ului.
    { 
        const uint8_t shape[] = {20, 8, 62, 8, 20}; // Memoreaza cele cinci coloane ale asteriscului.
        return shape[column]; // Intoarce coloana ceruta, nu o cifra din PIN.
    } 
    case '_': // Selecteaza cursorul de introducere.
        return 64; // Aprinde bitul 6 pe fiecare coloana, formand linia de jos.
    case '-': // Selecteaza minusul sau marcajul unei valori necunoscute.
        return 8; // Aprinde bitul 3, formand linia de la mijloc.
    case ':': // Selecteaza separatorul dintre ore/minute sau eticheta/valoare.
        return column == 2 ? 36 : 0; // Aprinde doua puncte numai in coloana centrala.
    case '.': // Selecteaza punctul folosit in meniuri si mesaje.
        return column == 2 ? 64 : 0; // Aprinde un singur pixel de jos in coloana centrala.
    case '/': // Selecteaza separatorul datei sau al alternativelor din meniu.
        return uint8_t(64U >> column); // Deplaseaza cate un bit pentru a forma o diagonala.
    case '#': // Selecteaza simbolul tastei pentru contrast, folosit numai ca eticheta.
        return column % 2 ? 127 : 20; // Alterneaza coloanele pline si cele doua linii transversale.
    case '?': // Selecteaza marcajul unei informatii necunoscute, inclusiv bateria.
    { 
        const uint8_t shape[] = {2, 1, 81, 9, 6}; // Defineste cele cinci coloane ale simbolului.
        return shape[column];                     // Intoarce coloana semnului de intrebare.
    } 
    default: // Trateaza spatiul si orice caracter nesuportat de fontul ASCII compact.
        return 0; // Deseneaza o coloana goala; nu interpreteaza secvente UTF-8 ca indici de tabel.
    } 
} 

// Trimite comenzile de configurare pe I2C.
bool Display::sendCommands(const uint8_t* commands, uint8_t length) // Defineste transportul comenzilor si parametrilor SSD1306.
{ 
    if (commands == nullptr || length > 31) // Protejeaza pointerul sursa si capacitatea pachetului de 32 B.
    { 
        lastError = EroareDisplay::ComenziInvalide; // Memoreaza eroarea parametrilor pachetului de comenzi.
        return false; // Nu efectueaza nicio transmisie pentru date invalide.
    } 

    uint8_t packet[32] = {}; // Rezerva pachetul pe stack; primul octet zero indica transmisie de comenzi.
    memcpy(packet + 1, commands, length); // Copiaza comenzile dupa octetul de control 0x00.
    if (HAL_I2C_Master_Transmit(&i2c, address, packet, uint16_t(length + 1), 10) != HAL_OK) // Transmite sincron cu timeout de 10 ms.
    { 
        ready = false; // Retrage confirmarea de functionare pana la o reinitializare ceruta de apelant.
        lastError = EroareDisplay::TransferComenziEsuat; // Retine esecul transmiterii comenzilor.
        return false; // Opreste operatia curenta la primul transfer esuat.
    } 
    return true; // Confirma doar transmiterea comenzilor, fara o autentificare sau decizie despre seif.
} 