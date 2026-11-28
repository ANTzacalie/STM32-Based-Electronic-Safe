
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




// METODA 01: Display - Creeaza obiectul si pregateste in RAM o imagine goala, cu octetul de control I2C. Nu configureaza pinii si nu trimite nimic pe OLED; hardware-ul este pregatit ulterior prin begin.
Display::Display()
{
   stergeBuffer(); // Pregateste in RAM o imagine goala; OLED-ul nu este inca initializat.
}

/// METODA 02: begin - Configureaza PB6/PB7 si I2C1, asteapta stabilizarea, cauta adresa OLED, trimite configurarea 128x32, afiseaza un cadru gol si porneste panoul. Returneaza true la succes sau false la primul esec, cu eroarea memorata.
bool Display::begin() // Leaga operatiile de pornire, fiecare avand propria functie mica.
{ 
    initializat = false; // Nu presupunem ca o initializare precedenta este inca valabila.
    initializarePini(); // Configureaza PB6 si PB7.

    if (initializareI2C() == false) // Verifica configurarea perifericului STM32.
    { //  cazul initializarii I2C esuate.
        return false; // Pastreaza eroarea stabilita de initializareI2C.
    } 
        
    HAL_Delay(STABILIZARE_OLED_MS); // Asteapta stabilizarea OLED doar la pornire, nu la fiecare ecran.
    if (cautaOLED() == false) // Verifica raspunsul la adresele posibile.
    { // cazul OLED-ului absent.
        return false; // Opreste pornirea si pastreaza diagnosticul.
    } 
    
    if (configureazaOLED() == false) // Trimite parametrii de configurare 128x32.
    { // cazul configurarii SSD1306 esuate.
        return false; // Nu afiseaza o imagine pe un modul neconfigurat.
    } 

    initializat = true; // Permite transmiterea imaginii si controlul panoului.
    stergeBuffer(); // Pregateste o imagine goala in RAM.
    
    if (afiseaza() == false) // Trimite imaginea goala o singura data, la pornire.
    { // cazul primului cadru netransmis.
        return false; // Nu declara initializare completa daca prima imagine a esuat.
    } 
    return setPornit(true); // Porneste panoul numai dupa configurare si transmiterea cadrului initial.
} 


// METODA 03: getLastError - Returneaza prin valoare codul memorat in ultimaEroare, din lista EroareDisplay. Nu trimite pe I2C, nu modifica obiectul si nu returneaza un pointer la text.
EroareDisplay Display::getLastError() const // Defineste metoda declarata in header pentru consultarea erorii.
{ // citirea erorii.
    return ultimaEroare; // Returneaza o copie din lista EroareDisplay, nu un pointer la text.
} 

// METODA 04: esteInitializat - Returneaza valoarea lui initializat: true daca afisajul este pregatit pentru comunicatie, false inainte de pornire sau dupa un defect de transfer. Nu verifica din nou cablurile si nu spune daca panoul este aprins.
bool Display::esteInitializat() const // Defineste consultarea starii comunicatiei.
{ // consultarea.
    return initializat; // True inseamna ca begin a reusit si nu a aparut ulterior o eroare de transfer.
} 

// METODA 05: setEnglish - Retine true pentru texte in engleza sau false pentru romana. Alegerea se aplica mesajelor cerute ulterior; metoda nu redeseneaza imediat ecranul si nu salveaza limba in Flash.
void Display::setEnglish(bool engleza) // Schimba numai alegerea textelor afisajului.
{ // alegerea limbii.
    limbaEngleza = engleza; // Retine preferinta; ecranul se redeseneaza cand apelantul cere un mesaj nou.
}

// METODA 06: stergeBuffer - Pune zero in cei 512 octeti ai imaginii din RAM si pastreaza prefixul I2C pentru pixeli. Nu trimite nimic: textul deja vizibil pe OLED ramane pana cand se apeleaza afiseaza.
void Display::stergeBuffer() // Inlocuieste clear cu un nume care spune clar ce se sterge.
{ // golirea imaginii locale.
    buffer[0] = CONTROL_PIXELI; // Primul octet ramane prefixul I2C pentru date grafice.
    for (uint16_t index = 1; index <= OCTETI_IMAGINE; index++) // Parcurge simplu cei 512 octeti de imagine, de la 1 la 512.
    { // stergerea unui octet din RAM.
        buffer[index] = 0; // Stinge in RAM cei opt pixeli ai octetului, fara comanda I2C.
    } 
}

// METODA 07: stergeRand - Goleste in RAM numai cei 128 de octeti ai randului primit, numerotat 0..3, pastrand ceilalti trei randuri. Nu transmite pe OLED; pentru un rand invalid memoreaza ParametruInvalid si iese.
void Display::stergeRand(uint8_t rand) // Sterge numai unul dintre cele patru randuri de opt pixeli.
{ //  verificarea randului primit.
    if (rand > ULTIMA_PAGINA) // Randurile permise sunt 0, 1, 2 si 3.
    { // Deschide cazul unui rand inexistent.
        setEroare(EroareDisplay::ParametruInvalid); // Retine eroarea, fara scriere in afara bufferului.
        return; // Paraseste metoda fara a modifica imaginea.
    } 

    uint16_t inceput = uint16_t(1 + rand * LATIME); // Sare peste prefix si peste paginile anterioare randului.
    for (uint16_t coloana = 0; coloana < LATIME; coloana++) // Parcurge numai cei 128 octeti ai randului ales.
    { // stergerea unei coloane de opt pixeli.
        buffer[inceput + coloana] = 0; // Goleste coloana randului, pastrand ceilalti trei randuri.
    } 
} 

// METODA 08: deseneazaPixel - Primeste coordonatele x/y si starea aprins, gaseste octetul si bitul pixelului, apoi aprinde sau stinge numai acel bit in RAM. Ignora coordonatele din afara panoului si nu transmite pe I2C.
void Display::deseneazaPixel(int x, int y, bool aprins) // Modifica un singur bit de imagine.
{ // verificarea coordonatelor.
    if (x < 0 || x >= LATIME || y < 0 || y >= INALTIME) // Verifica limitele inainte de accesul la tablou.
    { // cazul pixelului in afara panoului.
        return; // Ignora pixelul, permitand taierea sigura la margine.
    } 
    
    uint16_t index = uint16_t(1 + x + (y / 8) * LATIME); // Include octetul de control si gaseste pagina verticala a pixelului.
    uint8_t masca = mastiPixeli[y % 8]; // Alege unul dintre cei opt biti, folosind un tabel mic de valori explicite.
    
    if (aprins == true) // Alege aprinderea sau stingerea ceruta.
    { // ramura de aprindere.
        buffer[index] = uint8_t(buffer[index] | masca); // Aprinde numai bitul selectat; ceilalti pixeli raman la fel.
    } 
    else // Alege stingerea acelui pixel.
    { // ramura de stingere.
        buffer[index] = uint8_t(buffer[index] & uint8_t(~masca)); // Stinge numai bitul selectat, pastrand ceilalti pixeli.
    } 
} 

// METODA 09: scrieCaracter - Primeste un caracter si pozitia lui, transforma literele mici in mari si deseneaza cinci coloane de font plus una libera. Inlocuieste si pixelii stinsi ai celulei de 6x8, fara actualizare fizica.
void Display::scrieCaracter(int x, int y, char caracter) // Deseneaza o singura litera/cifra, in locul unei functii mari pentru tot textul.
{ // pregatirea caracterului.
    if (caracter >= 'a' && caracter <= 'z') // Recunoaste literele mici ASCII.
    { // conversia pentru refolosirea fontului.
        caracter = char(caracter - 'a' + 'A'); // Transforma litera mica in mare; nu pastram un al doilea font.
    } 

    for (uint8_t coloana = 0; coloana < 6; coloana++) // Parcurge cinci coloane de litera si o coloana libera.
    { // desenarea unei coloane.
        uint8_t valoare = 0; // Coloana libera si caracterele necunoscute pornesc goale.
       
        if (coloana < 5) // Numai primele cinci coloane apartin desenului caracterului.
        { // citirea fontului.
            valoare = coloanaCaracter(caracter, coloana); // Citeste o singura coloana din tabel.
        } 
        deseneazaColoana(x + coloana, y, valoare); // Deseneaza coloana prin metoda mica deja explicata.
    } 
} 

// METODA 10: scrieText - Parcurge sirul primit si apeleaza scrieCaracter pentru fiecare caracter, avansand cu sase pixeli. Se opreste la terminator sau marginea dreapta, ignora pointerul nul/originea invalida si modifica doar RAM-ul.
void Display::scrieText(int x, int y, const char* text) // Scrie un sir, apeland repetat functia pentru un singur caracter.
{ // verificarea sirului.
    if (text == nullptr || x < 0 || y < 0 || y >= INALTIME) // Nu citeste un pointer nul si nu foloseste o origine invalida.
    { // cazul argumentelor neutilizabile.
        return; // Iese fara citire invalida sau modificarea ecranului fizic.
    }

    uint16_t index = 0; // Incepe cu primul caracter al textului.
   
    while (text[index] != '\0' && x <= LATIME - 6) // Continua pana la sfarsitul textului sau marginea dreapta.
    { // scrierea caracterului curent.
        scrieCaracter(x, y, text[index]); // Deseneaza un singur caracter in RAM.
        x = x + 6; // Avanseaza pozitia cu latimea celulei caracterului.
        index++; // Trece la urmatorul caracter din sir.
    } 
} 

// METODA 11: scrieRand - Primeste un rand 0..3 si un text, sterge numai acel rand din RAM si scrie noul text la inaltimea corespunzatoare. Nu afiseaza inca; pentru un rand invalid retine eroarea si iese.
void Display::scrieRand(uint8_t rand, const char* text) // Inlocuieste continutul unui singur rand din RAM.
{ // verificarea randului.
    if (rand > ULTIMA_PAGINA) // Nu permite alte randuri decat 0..3.
    { // cazul randului invalid.
        setEroare(EroareDisplay::ParametruInvalid); // Memoreaza argumentul gresit.
        return; // Nu modifica memoria imaginii.
    } 
    
    stergeRand(rand); // Goleste doar cei 128 B ai randului care va fi inlocuit.
    scrieText(0, rand * 8, text); // Transforma numarul randului in coordonata verticala.
} 

// METODA 12: afiseaza - Verifica initializarea, selecteaza toate coloanele/paginile si trimite bufferul de 513 B: un octet de control plus 512 B de imagine. Returneaza succesul I2C si nu sterge imaginea dupa transmitere.
bool Display::afiseaza() // Trimite simplu tot cadrul, fara dirtyPages sau comparatii de cache.
{ // actualizarea OLED.
    if (verificaInitializare() == false) // Verifica o singura data ca putem comunica.
    { // cazul display-ului neinitializat sau cu eroare de transport.
        return false; // Nu sterge imaginea; doar refuza un transfer nesigur.
    } 

    uint8_t zona[] = { // Pregateste fereastra pentru intregul panou.
        CONTROL_COMENZI, // Pachetul contine comenzi.
        COMANDA_COLOANE, PRIMA_COLOANA, ULTIMA_COLOANA, // Foloseste coloanele 0..127.
        COMANDA_PAGINI, PRIMA_PAGINA, ULTIMA_PAGINA // Foloseste paginile 0..3, adica toate cele 32 de linii.
    };

    if (transmite(zona, uint16_t(sizeof zona)) == false) // Configureaza zona inaintea datelor.
    { // Deschide cazul in care zona nu a putut fi selectata.
        return false; // Nu trimite imaginea intr-o pozitie necunoscuta.
    } 
    
    return transmite(buffer, OCTETI_TRANSMISIE); // Trimite prefixul si cei 512 B; NU sterge bufferul dupa transmitere.
} 

// METODA: setLuminozitate - Primeste direct contrastul 0..255 si trimite numai comanda SSD1306 de contrast, dupa verificarea initializarii. Nu sterge si nu redeseneaza textul; returneaza succesul comunicatiei.
bool Display::setLuminozitate(uint8_t contrast) // Primeste direct o valoare de contrast 0..255, nu un indice de tablou.
{ // schimbarea contrastului.
    if (verificaInitializare() == false) // Nu trimite pe un periferic neinitializat; nu goleste ecranul.
    { // Deschide cazul in care nu putem comunica.
        return false; // Pastreaza imaginea si diagnosticul existent.
    } 
    uint8_t comenzi[] = {CONTROL_COMENZI, COMANDA_CONTRAST, contrast}; // Pregateste doar comanda de contrast si valoarea ceruta.
    return transmite(comenzi, uint16_t(sizeof comenzi)); // Trimite schimbarea fara clear, redesenare sau modificarea Password-ului.
} 

// METODA: luminozitateMedie - Apeleaza setLuminozitate cu valoarea CONTRAST_MEDIU, adica 128, si returneaza rezultatul. Nu citeste taste si nu modifica imaginea.
bool Display::luminozitateMedie() // O metoda mica pentru treapta medie.
{ // cererea treptei medii.
    return setLuminozitate(CONTRAST_MEDIU); // Nu numara apasari pe # si nu decide standby-ul.
} 





// METODA: setPornit - Trimite comanda ON daca pornit=true sau OFF daca este false, dupa verificarea initializarii. Nu intrerupe alimentarea fizica si nu sterge bufferul imaginii; returneaza succesul transferului.
bool Display::setPornit(bool pornit) // Controleaza doar afisarea panoului, nu alimentarea fizica a modulului.
{ // alegerea comenzii ON/OFF.
    if (verificaInitializare() == false) // Cere comunicatie pregatita.
    { // cazul unei comunicatii indisponibile.
        return false; // Nu trimite si nu sterge bufferul.
    } 
    
    uint8_t comenzi[] = {CONTROL_COMENZI, COMANDA_OPRIRE}; // Pregateste implicit comanda de stingere a panoului.
    if (pornit == true) // Verifica daca apelantul a cerut aprinderea.
    { // cazul pornirii.
        comenzi[1] = COMANDA_PORNIRE; // Inlocuieste comanda OFF cu ON.
    }
    
    return transmite(comenzi, uint16_t(sizeof comenzi)); // Trimite numai comanda, fara sa schimbe imaginea din RAM.
} 

// METODA: initializarePini - Activeaza ceasurile GPIOB/AFIO, pastreaza I2C1 neremapat si configureaza PB6/PB7 ca alternate function open-drain prin HAL. 
void Display::initializarePini() // Configureaza numai cei doi pini ai display-ului.
{ // configurarea GPIO prin HAL.
    __HAL_RCC_GPIOB_CLK_ENABLE(); // Activeaza ceasul portului B.
    __HAL_RCC_AFIO_CLK_ENABLE(); // Permite configurarea functiei alternative a perifericului.
    __HAL_AFIO_REMAP_I2C1_DISABLE(); // Pastreaza I2C1 pe PB6/PB7, nu pe pinii remapati.
    GPIO_InitTypeDef pini = {}; // Structura impusa de HAL, nu o structura proprie de logica.
    pini.Pin = GPIO_PIN_6 | GPIO_PIN_7; // Selecteaza PB6=SCL si PB7=SDA impreuna.
    pini.Mode = GPIO_MODE_AF_OD; // Selecteaza functia alternativa open-drain folosita de I2C.
    pini.Pull = GPIO_NOPULL; // Nu cere rezistente interne; pull-up-urile externe ale I2C trebuie verificate pe placa.
    pini.Speed = GPIO_SPEED_FREQ_HIGH; // Configureaza viteza de comutare a pinilor, nu frecventa magistralei.
    HAL_GPIO_Init(GPIOB, &pini); // Aplica aceasta configuratie pe portul B prin HAL.
}

// METODA: initializareI2C - Activeaza I2C1, completeaza handle-ul HAL pentru 100 kHz si adrese pe sapte biti, apoi aplica HAL_I2C_Init. Returneaza true la succes sau false cu InitializareI2CEsuata memorata; nu detecteaza OLED-ul.
bool Display::initializareI2C() // Pregateste perifericul I2C1 folosit exclusiv de OLED in acest modul.
{ // incepe configurarea handle-ului HAL.
    __HAL_RCC_I2C1_CLK_ENABLE(); // Activeaza ceasul perifericului I2C1.
    i2c.Instance = I2C1; // Selecteaza perifericul, fara scrieri manuale la adrese de registre.
    i2c.Init.ClockSpeed = VITEZA_I2C; // Cere 100 kHz, valoare numita in header.
    i2c.Init.DutyCycle = I2C_DUTYCYCLE_2; // Completeaza parametrul cerut de HAL.
    i2c.Init.OwnAddress1 = 0; // Nu folosim o adresa proprie a STM32 ca dispozitiv slave.
    i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; // Selecteaza adresarea I2C pe sapte biti.
    i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE; // Nu folosim o a doua adresa proprie.
    i2c.Init.OwnAddress2 = 0; // Initializeaza si valoarea adresei proprii secundare.
    i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; // Dezactiveaza raspunsul la general-call.
    i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE; // Lasa optiunea NoStretch dezactivata.
    
    if (HAL_I2C_Init(&i2c) != HAL_OK) // Verifica rezultatul configurarii magistralei.
    { // incepe tratarea configurarii esuate.
        setEroare(EroareDisplay::InitializareI2CEsuata); // Memoreaza eroarea de initializare, nu o eroare de Password.
        return false; // Opreste pornirea display-ului.
    } 

    return true; // Confirma configurarea I2C, nu si prezenta OLED-ului.
} 

// METODA: cautaOLED - Incearca adresele numite 0x3C si 0x3D, transformate in format HAL, si pastreaza adresa care raspunde. Returneaza true la primul ACK; altfel memoreaza DisplayFaraRaspuns si returneaza false.
bool Display::cautaOLED() // Incearca cele doua adrese definite explicit in header.
{ // detectarea adresei OLED.
    adresaHAL = uint16_t(ADRESA_OLED_PRINCIPALA * 2); // HAL cere adresa pe sapte biti deplasata cu un bit, echivalent cu inmultirea cu doi.
    
    if (HAL_I2C_IsDeviceReady(&i2c, adresaHAL, 1, TIMEOUT_I2C_MS) == HAL_OK) // Incearca prima adresa cu timeout finit.
    { // cazul adresei principale confirmate.
        return true; // Pastreaza adresa care a raspuns si termina cautarea.
    } 
    adresaHAL = uint16_t(ADRESA_OLED_ALTERNATIVA * 2); // Pregateste in format HAL a doua adresa posibila.
   
    if (HAL_I2C_IsDeviceReady(&i2c, adresaHAL, 1, TIMEOUT_I2C_MS) == HAL_OK) // Incearca si adresa alternativa.
    { // cazul adresei alternative confirmate.
        return true; // Pastreaza adresa alternativa si termina cautarea.
    } 
    setEroare(EroareDisplay::DisplayFaraRaspuns); // Nicio adresa nu a raspuns; poate fi alimentare, cablaj sau alt modul.
    return false; // Nu continuam initializarea fara un raspuns de la dispozitiv.
} 

// METODA: configureazaOLED - Construieste pachetul de comenzi numite pentru SSD1306 128x32: orientare, adresare, contrast initial si parametrii panoului. Il trimite prin transmite si returneaza rezultatul; nu porneste aici logica seifului.
bool Display::configureazaOLED() // Pregateste comenzile SSD1306 pentru un panou de 128x32.
{ // incepe configurarea controlerului OLED, nu a logicii seifului.
    uint8_t comenzi[] = { // Lista de comenzi foloseste numai numele definite in header.
        CONTROL_COMENZI, // Primul octet spune ca urmeaza comenzi.
        COMANDA_OPRIRE, // Opreste temporar panoul pe durata configurarii.
        COMANDA_CEAS, VALOARE_CEAS, // Configureaza ceasul intern al SSD1306.
        COMANDA_MULTIPLEX, MULTIPLEX_32_LINII, // Selecteaza cele 32 de linii ale display-ului.
        COMANDA_OFFSET, OFFSET_ZERO, // Nu deplaseaza imaginea pe verticala.
        COMANDA_LINIE_START, // Incepe imaginea de la prima linie.
        COMANDA_POMPA, POMPA_ACTIVA, // Activeaza pompa interna a modulului.
        COMANDA_ADRESARE, ADRESARE_ORIZONTALA, // Selecteaza organizarea transferului complet.
        COMANDA_ORIENTARE_X, // Configureaza orientarea pe orizontala.
        COMANDA_ORIENTARE_Y, // Configureaza orientarea pe verticala.
        COMANDA_CONFIG_COM, CONFIG_COM_32, // Selecteaza conexiunile COM pentru 128x32.
        COMANDA_CONTRAST, CONTRAST_MINIM, // Porneste cu contrast minim, ca preferinta initiala.
        COMANDA_PREINCARCARE, VALOARE_PREINCARCARE, // Configureaza intervalul de preincarcare.
        COMANDA_VCOMH, VALOARE_VCOMH, // Configureaza nivelul VCOMH ca in modelul partajat.
        COMANDA_AFISARE_RAM, // Foloseste imaginea din memoria OLED.
        COMANDA_NORMAL, // Afiseaza normal, fara inversare.
        COMANDA_OPRIRE_SCROLL // Nu deplaseaza automat imaginea.
    };
    return transmite(comenzi, uint16_t(sizeof comenzi)); // Trimite secventa; sizeof numara octetii tabloului.
} 

// METODA: transmite - Primeste un pachet intern si lungimea lui si il trimite prin HAL_I2C_Master_Transmit cu timeout de 100 ms. La esec retrage initializat si retine timeout/comenzi/imagine; la succes retine NicioEroare si returneaza true.
bool Display::transmite(uint8_t* date, uint16_t lungime) // Singura functie care transmite un pachet de comenzi sau pixeli.
{ // incepe transportul I2C.
    HAL_StatusTypeDef rezultat = HAL_I2C_Master_Transmit(&i2c, adresaHAL, date, lungime, TIMEOUT_I2C_MS); // Asteapta sincron transferul, cel mult timpul numit in header.
    
    if (rezultat != HAL_OK) // Verifica daca HAL a raportat o problema.
    { // deschide tratarea erorii de transport.
        initializat = false; // Cere reinitializare inainte de urmatoarele transmisii publice.
        
        if (rezultat == HAL_TIMEOUT) // Separa expirarea asteptarii de celelalte erori HAL.
        { // cazul timeout-ului.
            setEroare(EroareDisplay::TimeoutI2C); // Memoreaza eroarea de timp expirat.
        }
        
        else if (date[0] == CONTROL_COMENZI) // Prefixul arata daca pachetul continea comenzi.
        { // cazul unei comenzi netransmise.
            setEroare(EroareDisplay::TransferComenziEsuat); // Memoreaza esecul comenzilor SSD1306.
        } 
        
        else // Celalalt pachet folosit de clasa contine imaginea.
        { // cazul imaginii netransmise.
            setEroare(EroareDisplay::TransferImagineEsuat); // Memoreaza esecul transmiterii pixelilor.
        } 
        
        return false; // Metoda apelanta afla ca transferul nu s-a incheiat cu succes.
    }

    setEroare(EroareDisplay::NicioEroare); // Marcheaza transferul reusit.
    return true; // Confirma transferul, nu starea mecanica sau autorizarea seifului.
} 




///////////////////////////////////

//metoda  - setBrightness: declarata in Display.h, la acelasi numar
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

//metoda  - clear: declarata in Display.h, la acelasi numar
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

 


// METODA - update: declarata in include/Display.h, la acelasi numar.
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

// METODA 18 - showCardReading: declarata in include/Display.h, la acelasi numar.
bool Display::showCardReading() // Defineste ecranul citirii cardului in curs.
{ 
    return showMessage(text("CITIRE CARD RFID", "READING RFID CARD"), text("ASTEPTATI CITIREA", "PLEASE WAIT"), "", ""); // Afiseaza asteptarea, fara comenzi catre cititorul RFID.
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