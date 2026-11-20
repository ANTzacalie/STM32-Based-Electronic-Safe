
#include <Cititor_RFID.h>


// Se executa utomat la creearea unui obiect de tip Cititor_RFID
Cititor_RFID::Cititor_RFID() 
{

    // conf. pini necesari mfrc522
    initializarePini();

    // configurare periferic SPI1
    initializareSPI();

    // daca nu exista nicio eroare, putem continua la initializarea antenei;
    if(getUltimaEroare() == EroareRFID:: NicioEroare)
    {
        activareAntena();
        /*
        - TxASKReg (0x15), bitul 6:
        activam modulatia 100% ASK pentru comunicarea Type A.
        - Conform datasheet pagina 51;
        */
        scriereRegistru(0x15, 0x40);
    }

}

/*
 * Configureaza pinii GPIO ai modulului MFRC522:
   - PA4 - NSS (Slave Select), activ LOW.
   - PB5 - RESET, activ LOW.
   - PA5 - SCK:  Clock generat de microcontroller.
   - PA6 - MISO: Date primite de la MFRC522.
   - PA7 - MOSI: Date trimise catre MFRC522.
*/
void Cititor_RFID::initializarePini()
{

    // Activam ceasul pentru porturile A SI B [GPIO]
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Setam pini NSS SI RESET pentru nivelul HIGH;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

    // Structura HAL pentru configurarea pinilor;
    GPIO_InitTypeDef GPIO_InitStruct = {};

    // Configutam GPIO_InitStruct cu setarile PIN_4 ca iesire digitala;
    GPIO_InitStruct.Pin   = GPIO_PIN_4;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP; // Push-Pull output only;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;  // Nu activam rezistente de pull-up/pull-down;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // Setam folosind HAL in registrele GPIO ale MICROCONTROLLERULUI configuratia pentru PA4 pe portul A;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configuram PB5 la fel ca si PA4, iesire digitala;
    // Asa ca nu mai trebuie sa creeam alt struct, doar schimbam PIN;
    GPIO_InitStruct.Pin = GPIO_PIN_5;

    // Setam cu HAL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Pentru interfata SPI;
    GPIO_InitTypeDef SPI_GPIO_Init = {};

    // PA5 si PA7 sunt iesiri controlat de perifericul SPI_1;
    // Merg ambii initializati deodata pt ca sunt din acelasi port(A);
    SPI_GPIO_Init.Pin   = GPIO_PIN_5 | GPIO_PIN_7;
    // alternate push_pull, microcontrollerul permite SPI sa controleze pinii PA5&7, fara sa schimbam manual starea lor prin HAL;
    SPI_GPIO_Init.Mode  = GPIO_MODE_AF_PP; 
    SPI_GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH; // viteza mare

    // Aplicam configuratia pentru pa5&7;
    HAL_GPIO_Init(GPIOA, &SPI_GPIO_Init);

    // PA6 este intrare;
    SPI_GPIO_Init.Pin  = GPIO_PIN_6;
    SPI_GPIO_Init.Mode = GPIO_MODE_INPUT; // doar intrare;
    SPI_GPIO_Init.Pull = GPIO_NOPULL;

    // Aplicam configuratia pentru PA6;
    HAL_GPIO_Init(GPIOA, &SPI_GPIO_Init);

}

/*
  - Conform datasheetulil MFRC522, sectiunea 8.1.2, cititorul comunica prin SPI
  si functioneaza ca Slave, Microcontrollerul fiind necesar sa fie Master, adica el controleaza comunicatia.

  - Initializarea perifericului SPI1.
  - Configureaza SPI1 in modul Master,
  pentru comunicarea cu MFRC522.

  [Parametru] [Valoare]	[Explicație]
   Mode	       MASTER	 MCU controlează comunicația
   Direction   2LINES	 Transmitere și recepție
   DataSize	   8BIT	     Transferăm câte 8 biți
   CPOL	       LOW	     Ceasul este LOW în repaus
   CPHA	       1EDGE	 Citirea pe primul front
   NSS	       SOFT	     Controlăm manual PA4
   Prescaler   16	     Divizăm frecvența SPI
   FirstBit	   MSB	     Transmitem bitul cel mai semnificativ primul

- CPOL = 0 și CPHA = 0 reprezintă SPI Mode 0, compatibil cu MFRC522.

SPI FULL DUPLEX
*/
void Cititor_RFID::initializareSPI()
{

    // Activam ceasul pentru perifericul SPI1;
    __HAL_RCC_SPI1_CLK_ENABLE();

    // Selectam perifericul SPI1 in hspi;
    hspi1.Instance = SPI1;

    // Punem microcontrollerul ca master;
    hspi1.Init.Mode = SPI_MODE_MASTER;

    // Transmitem si receptionam date pe 8 biti(1 byte);
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;

    // SPI Mode 0 -> CPOL = 0, CPHA = 0;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase    = SPI_PHASE_1EDGE;

    // Controlam manual NSS pron GPIO PA4;
    hspi1.Init.NSS = SPI_NSS_SOFT;

    // Frecventa SPI = PCLK2 / 16;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;

    // Transmitem mai intai bitul cel mai semnificativ MSB;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;

    // Dezactivam protocolul TI;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;

    // Nu folosim CRC hardware pentru transferul SPI, asa ca il dezactivam;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7; // val de configurare, neutilizata;

    // Aplicam configuratia pe registrele SPI1 cu HAL si verificam rezultatul;
    if(HAL_SPI_Init(&hspi1) != HAL_OK) // daca HAL_SPI_Init nu intoarce HAL_OK la configuratia hspi1, inseamna ca SPI1 a esuat;
    {
        setEroare(EroareRFID::InitializareEsuata);
    }

}

/*
  - Citeste un registru intern al MFRC522.
  - Comunicatia foloseste SPI1 si NSS controlat manual.
  - Intr-o citire de registru, primul octet transmis este 
  adresa, iar al doilea generează impulsurile de ceas necesare 
  pentru primirea datelor. Primul octet recepționat este ignorat.
 */
bool Cititor_RFID::citireRegistru(uint8_t reg, uint8_t &val) 
{

    /*
       Formatul adresei SPI pentru citire:
      
       Bit 7    = 1 (operatie READ)
       Biti 6:1 = adresa registrului
       Bit 0    = 0

    */
   // Buffer pentru datele transmise;
    uint8_t dateTrimise[2] ={

        static_cast<uint8_t>(0x80 | ((reg & 0x3F) << 1)), 0x00

    };

    // Buffer pentru datele receptionate;
    uint8_t datePrimite[2] = {0};

    // Selectam MFRC522: NSS activ LOW'
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

    /*
      Trimitem doi octeti:
      1. Adresa registrului.
      2. Octet "dummy" pentru a genera ceasul SPI necesar receptionarii valorii registrului.
    
     DIN HAL:
     Transmit and Receive an amount of data in blocking mode.

     Parameters:
        hspi – pointer to a SPI_HandleTypeDef structure that contains the configuration information for SPI module.
        pTxData – pointer to transmission data buffer
        pRxData – pointer to reception data buffer
        Size – amount of data to be sent and received
        Timeout – Timeout duration

     Return values:
     HAL status
    */
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(
        &hspi1, // adm. SPI
        dateTrimise,
        datePrimite,
        2, // 2 bytes
        100 // 100ms timeout
    );

    // Incheiem tranzactia SPI: NSS devine HIGH;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

    // Verificam daca transferul SPI a esuat;
    if(status != HAL_OK) {
        
        if(status == HAL_TIMEOUT) 
            setEroare(EroareRFID::Timeout);
        else 
            setEroare(EroareRFID::EroareSPI);

        return false;
    }

    // Al doilea octet(byte) contine valoarea registrului;
    val = datePrimite[1];    

    // Transferul SPI sa executat cu succes;
    setEroare(EroareRFID::NicioEroare);

    return true;

}

/*
  - Citim un registru prin functia SPI
  deja implementata in clasa.
 */
uint8_t Cititor_RFID::citesteRegistruTest(uint8_t adresa)
{
    uint8_t valoare = 0xFF;

    // Citim registrul solicitat.
    if (!citireRegistru(adresa, valoare))
    {
        // Transfer SPI esuat, folosim 0xFF ca identificator.
        return 0xFF;
    }

    return valoare;
}


/*
    - Scrie o valoare intrun registru MFRC522,
    - Formatul SPI -> bit 7 = 0 pentru WRITE, biti 6:1 = adresa, bit 0 = 0.
*/
bool Cititor_RFID::scriereRegistru(uint8_t reg, uint8_t val) 
{

    uint8_t date[2] = {
        static_cast<uint8_t>((reg & 0x3F) << 1), val
    };    

    // Selectam cititorul: NSS = LOW;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

    // Trimitem adresa si valoarea registrului;
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, date, 2, 100); // size 2bytes 100ms tran. timeout

    // Eliberam cititorul: NSS = HIGH;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

    // Verificam rezultatul transferului catre MFRC522;
    if(status != HAL_OK) 
    {

        if(status == HAL_TIMEOUT)
            setEroare(EroareRFID::Timeout);
        else 
            setEroare(EroareRFID::EroareSPI);

        return false;

    }

    return true;

}

/*
 - Citeste registrul VersionReg (adresa 0x37)
 - pentru identificarea cititorului MFRC522.

    Va returna conform doc. NXP:
        1.0x91 pentru MFRC522 V1.0;
        2.0x92 pentru MFRC522 V2.0;
        3.Pot fi si alti bytes, precum 0x82(Clona) etc....
*/
uint8_t Cititor_RFID::citesteVersiune() {

    uint8_t versiune = 0;

    // Executam citirea registrului VersionReg;
    if(!citireRegistru(0x37, versiune)) 
    {
        // eroarea spi a fost deja inreg etc...;
        return 0;
    }

    if(versiune != 0x91 && versiune != 0x92 && 
       versiune != 0x82 && versiune != 0x90 &&
       versiune != 0x88 && versiune != 0x89 && versiune != 0xB2)
    {
        setEroare(EroareRFID::InitializareEsuata);
    }

    return versiune;
    
}

/*
    - Va memora un nou card;
*/
uint8_t Cititor_RFID::stocareCard()
{
    // TODO: Initializare SPI si MFRC522.

    return 0;
}

/*
 - Va detecta un card, va autentifica sectorul
 folosind Crypto1 si va compara credentialul citit cu cel salvat in memoria microcontrollerului.
 */
uint8_t Cititor_RFID::verificaCard()
{
   // TODO
    return 0;
}

// Seteaza eroarea;
void Cititor_RFID::setEroare(EroareRFID eroare)
{
    ultimaEroare = eroare;
}

// TODO
EroareRFID Cititor_RFID::handleEroare() 
{

    switch (ultimaEroare)
    {
        case EroareRFID::NicioEroare:
            // Nu este necesara nicio actiune.
            break;

        case EroareRFID::EroareSPI:
        case EroareRFID::Timeout:
            // TODO: Recuperare comunicatie SPI.
            break;

        case EroareRFID::AutentificareEsuata:
        case EroareRFID::CardNeautorizat:
            // Accesul ramane blocat.
            break;

        default:
            // TODO: Gestionarea celorlalte erori.
            break;
    }

    return ultimaEroare;

}

/*
 - Verifica scrierea si citirea registrului 0x2D ca si test;
 
 - Returneaza:
 - 1 - Valorile scrise au fost citite corect.
 - 0 - Testul a esuat.
 */
uint8_t Cititor_RFID::testComunicareSPI()
{

    const uint8_t registruTest = 0x2D;

    uint8_t val_In = 0; // valoarea initiala
    uint8_t val_Rd = 0; // valoarea citita

    // Salvam continutul initial al registrului;
    if(!citireRegistru(registruTest, val_In))
        return 0;
    
    // Scriem 0x55 si verificam citirea;
    bool succes = scriereRegistru(registruTest, 0x55) && citireRegistru(registruTest, val_Rd) && (val_Rd == 0x55);

    // Daca prima verificare a reusit, testam 0xAA;
    if (succes)
    {
        succes = scriereRegistru(registruTest, 0xAA) && citireRegistru(registruTest, val_Rd) &&  (val_Rd == 0xAA);
    }

    // Restauram valoarea initiala a registrului;
    bool restaurat = scriereRegistru(registruTest, val_In);

    if(!succes || !restaurat)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    setEroare(EroareRFID::NicioEroare);
    return 1;

}

/*
 - Conform datasheet NXP, pagina 50, registrul TxControlReg are adresa 0x14.
 - Aici avem 2 biti:
    b0 -> Tx1RFEn , activeaza driverul antenei TX1
    b1 -> Tx2RFEn , activeaza driverul antenei TX2
 - Pentru activarea antenei(completa), ambii biti trebuie sa fie 1, doar acesti biti din registru;
*/

bool Cititor_RFID::activareAntena() 
{

    const uint8_t registruTxControl = 0x14;
    const uint8_t mascaActivareRF = 0x03;

    uint8_t valoareRegistru = 0;

    // Citim configuratia actuala a antenei;
    if(!citireRegistru(registruTxControl, valoareRegistru))
    {
        return false;
    }

    // Verificam daca driverele sunt deja active;
    if((valoareRegistru & mascaActivareRF) != mascaActivareRF)
    {
        // Setam biti b0 si b1 folosind operatia OR; ceilalti biti raman neschimbati;
        /*
            Inainte:   1000 0000   (0x80)
            Masca:     0000 0011   (0x03)
                       ---------
            Rezultat:  1000 0011   (0x83), sa facut efectiv operatia SAU pe biti;
        */
        valoareRegistru |= mascaActivareRF;
        
        // Scriem noua configuratie in MFRC522;
        if(!scriereRegistru(registruTxControl, valoareRegistru))
        {
            return false;
        }

    }

    // Recitim registrul pentru configurare;
    if(!citireRegistru(registruTxControl, valoareRegistru))
    {
        return false;
    }

    // Verificam daca TX1 si TX2 sunt activate;
    if((valoareRegistru & mascaActivareRF) != mascaActivareRF)
    {
        setEroare(EroareRFID::InitializareEsuata);
        return false;
    }

    return true;

}

/*
 REQA solicită răspunsul unui card ISO/IEC 14443 Type A aflat în câmpul antenei.
 REQA = 0x26, transmis pe 7 biți, cardul RFID răspunde prin ATQA, care conține 2 octeți.
 
 - Detecteaza un card ISO14443A prin comanda REQA.
 - Foloseste FIFO-ul si motorul Transceive al MFRC522.
 - *Nu folosim pinul IRQ, ci verificam ComIrqReg.
 - Toate registrele sunt incarcate cu comenzi conform datasheet, pagina 70, tabelul 149;
*/

uint8_t Cititor_RFID::detecteazaCard(uint8_t atqa[2]) 
{

    // Initializam bufferul de raspuns;
    atqa[0] = 0; // oct 1
    atqa[1] = 0; // oct 2

    /*
      Pregatirea transmisiei conform cu datasheetul MFRC522, reg_init:
        1. CommandReg    = 0x01
        2. ComIrqReg     = 0x04
        3. FIFODataReg   = 0x09
        4. FIFOLevelReg  = 0x0A
        5. BitFramingReg = 0x0D
    */
    if(!scriereRegistru(0x01, 0x00) || // state Idle
       !scriereRegistru(0x04, 0x7F) || // Stergem IRQ vechi
       !scriereRegistru(0x0A, 0x80) || // Golim FIFO
       !scriereRegistru(0x09, 0x26) || // REQA
       !scriereRegistru(0x0D, 0x07) || // Transmitem 7 biti
       !scriereRegistru(0x01, 0x0C) || // state Transceive(0x0C)              | comanda 0x0C si bitul StartSend,
       !scriereRegistru(0x0D, 0x87))   // Incepem trimiterea(bitul StartSend) | pentru a porni transmisia radio;
    {
        return 0;
    }

    /*
     - Asteptam raspunsul cardului, maximum 30 ms conform datasheet;
     
     - ComIrqReg:
        -> bit 5 = RxIRq (raspuns receptionat)
        -> bit 1 = ErrIRq (eroare)
        -> bit 0 = TimerIRq (timeout hardware)
    */
    uint8_t irq = 0;
    bool raspunsPrimit = false;
    bool comunicatieOK = true;

    uint32_t timpStart = HAL_GetTick(); // ne intoarce timpul curent in ms;

    while ((HAL_GetTick() - timpStart) < 30U)
    {
        // nu sa putu citi registrul irq;
        if (!citireRegistru(0x04, irq))
        {
            comunicatieOK = false;
            break;
        }

        // sa primit raspunsul;
        if (irq & 0x20)
        {
            raspunsPrimit = true;
            break;
        }

        // intrerupere / fail;
        if (irq & 0x03)
        {
            break;
        }
    }

    // Oprim mai intai transmisia si revenim la Idle;
    if(!scriereRegistru(0x0D, 0x07) || !scriereRegistru(0x01, 0x00))
    {
        return 0;
    }

    if(!comunicatieOK)
        return 0;
    
    if(!raspunsPrimit)
    {
        if(irq & 0x02) 
           setEroare(EroareRFID::EroareCitire);
        else 
           setEroare(EroareRFID::CardAbsent);
        
        return 0;
    }

    // Verificam erorile si lungimea raspunsului;
    uint8_t erori    = 0;
    uint8_t nrOcteti = 0;
    uint8_t control  = 0;

    if(!citireRegistru(0x06, erori) || !citireRegistru(0x0A, nrOcteti) || !citireRegistru(0x0C, control))
    {
        return 0;
    }

    /*
     - ATQA trebuie sa aiba exact 16 biti:
       -> 2 octeti in FIFO
       -> 0 biti suplimentari in ultimul octet.
     - 0xDF verifica toate erorile in afara de reserved;
    */
    if((erori & 0xDF) != 0 || nrOcteti != 2 || (control & 0x07) != 0)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Citim cei doi octeti ATQA din FIFO;
    if(!citireRegistru(0x09, atqa[0]) || !citireRegistru(0x09, atqa[1]))
    {
        return 0;
    }

    setEroare(EroareRFID::NicioEroare);
    return 1;

}
   
/*
  - Citeste identificatorul cardului la Cascade Level 1.
  - Se apeleaza dupa ce cardul a raspuns la REQA.
 
  - *Momentan presupunem un singur card in campul RF.
 */
uint8_t Cititor_RFID::citesteUID_CL1(uint8_t uidCL1[5])
{

    if(uidCL1 == nullptr)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Golim bufferul uidCL1 furnizat;
    for(uint8_t i = 0; i < 5; i++)
        uidCL1[i] = 0;

    /*
     Pregatim MFRC522 pentru anticollision:
       - Oprim comenzile anterioare.
       - Stergem intreruperile memorate.
       - Golim FIFO.
       - Trimitem SEL_CL1 si NVB.
     */
    if (!scriereRegistru(0x01, 0x00) ||  // Idle
        !scriereRegistru(0x04, 0x7F) ||  // Sterge IRQ
        !scriereRegistru(0x0A, 0x80) ||  // Goleste FIFO
        !scriereRegistru(0x0E, 0x80) ||  // CollReg
        !scriereRegistru(0x09, 0x93) ||  // SEL_CL1
        !scriereRegistru(0x09, 0x20) ||  // NVB
        !scriereRegistru(0x0D, 0x00) ||  // 8 biti, 1 octet
        !scriereRegistru(0x01, 0x0C) ||  // Transceive
        !scriereRegistru(0x0D, 0x80))    // StartSend
    {
        return 0;
    }

    // Asteptam raspunsul cardului;
    // Folosim polling pe ComIrqReg, maxim 30ms;
    uint8_t irq = 0;
    uint32_t timpStart = HAL_GetTick();

    while ((HAL_GetTick() - timpStart) < 30U)
    {
        if(!citireRegistru(0x04, irq))
        {
            // Oprim comanda in cazul erorri SPI;
            scriereRegistru(0x0D, 0x00);
            scriereRegistru(0x01, 0x00);
            return 0;
        }

        // RxIRq, ErrIRq sau TimerIRq;
        if(irq & 0x23)
            break;
    }
    
    // Oprim transmisia si revenim la Idle;
    if(!scriereRegistru(0x0D, 0x00) || !scriereRegistru(0x01, 0x00))
    {
        return 0;
    }

    // Verificam daca am primit raspuns;
    if((irq & 0x20) == 0)
    {
        if(irq & 0x02) 
            setEroare(EroareRFID::EroareCitire);
        else 
            setEroare(EroareRFID::Timeout);

        return 0;
    }

    // Citim registrele de stare ale raspunsului;
    uint8_t erori   = 0;
    uint8_t lungime = 0;
    uint8_t control = 0;

    // conform NXP datasheet
    if(!citireRegistru(0x06, erori) || !citireRegistru(0x0A, lungime) || !citireRegistru(0x0C, control))
    {
        return 0;
    }

    /*
      Verificam:
       - Sa nu existe erori de protocol/coliziune.
       - Sa avem exact 5 octeti.
       - Ultimul octet sa contina 8 biti valizi.
       0xDF verifica toate erorile BufferOvlf, ColErr, ParityErr, ProtocolErr, nu este 
       verificat reserved, masca bitilor se gaseste la pagina 42;
    */

    // conform NXP datasheet, 0x07 izoleaza exact ultimi trei biti din control, care ei trebuie sa fie zero;
    if((erori & 0xDF) != 0 || lungime != 5 || (control & 0x07) != 0)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Citim cei 5 octeti din FIFO;
    for(uint8_t i = 0; i < 5; i++)
    {
        if(!citireRegistru(0x09, uidCL1[i]))
            return 0;
    }

    /*
      Verificam BCC;
      Conform datasheet XOR intre primii 4 octeti trebuie sa fie egal cu al cincilea octet;
    */
    uint8_t bcc = uidCL1[0] ^ uidCL1[1] ^ uidCL1[2] ^ uidCL1[3]; // Operatia XOR(^);

    if(bcc != uidCL1[4])
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    setEroare(EroareRFID::NicioEroare);
    return 1;

}


/*
  - Calculeaza CRC_A folosind hardware-ul MFRC522;
   - ModeReg        = 0x11
   - CommandReg     = 0x01
   - DivIrqReg      = 0x05
   - FIFODataReg    = 0x09
   - FIFOLevelReg   = 0x0A
   - CRCResultRegL  = 0x22
   - CRCResultRegH  = 0x21
 */
bool Cititor_RFID::calculeazaCRC_A(const uint8_t *date, uint8_t lungime, uint8_t crc[2])
{

    // Verificam argumentele si capacitatea FIFO;
    if (date == nullptr || crc == nullptr || lungime == 0 || lungime > 64)
    {
        setEroare(EroareRFID::EroareCitire);
        return false;
    }

    // Citim configuratia actuala a registrului ModeReg;
    uint8_t mode = 0;

    if (!citireRegistru(0x11, mode))
        return false;

    /*
     - Bitii 1:0 = 01 selecteaza valoarea initiala;
     - CRC = 0x6363, folosita pentru CRC_A;
     - Pastram ceilalti biti nemodificatip
    */
    mode = (mode & 0xFC) | 0x01;

    if (!scriereRegistru(0x11, mode) ||
        !scriereRegistru(0x01, 0x00) || // Idle
        !scriereRegistru(0x05, 0x04) || // Stergem CRCIRq
        !scriereRegistru(0x0A, 0x80))   // Golim FIFO
    {
        return false;
    }

    // Introducem datele in FIFO-ul MFRC522;
    for (uint8_t i = 0; i < lungime; i++)
    {
        if (!scriereRegistru(0x09, date[i]))
            return false;
    }

    // Pornim coprocesorul CRC prin: CalcCRC = 0x03;
    if (!scriereRegistru(0x01, 0x03))
        return false;

    uint8_t irq = 0;
    bool calculTerminat = false;
    uint32_t timpStart = HAL_GetTick();

    // Asteptam maximum 30 ms finalizarea calculului;
    while ((HAL_GetTick() - timpStart) < 30U)
    {
        if (!citireRegistru(0x05, irq))
        {
            scriereRegistru(0x01, 0x00);
            return false;
        }

        // Bitul 2 (CRCIRq) indica finalizarea calculului;
        if ((irq & 0x04) != 0)
        {
            calculTerminat = true;
            break;
        }
    }

    // Incheiem comanda CalcCRC;
    if (!scriereRegistru(0x01, 0x00))
        return false;

    if (!calculTerminat)
    {
        setEroare(EroareRFID::Timeout);
        return false;
    }

    // Citim rezultatul: mai intai LSB, apoi MSB;
    if (!citireRegistru(0x22, crc[0]) || !citireRegistru(0x21, crc[1]))
    {
        return false;
    }

    setEroare(EroareRFID::NicioEroare);
    return true;
}

/*

 - Comanda SELECT transmite 0x93, 0x70, cei 5 octeți UID CL1 + BCC și cei 2 octeți CRC_A.
 - Cardul răspunde cu 3 octeți: SAK + CRC_L + CRC_H.
 - Pentru un MIFARE Classic 1K cu UID complet după CL1, ne așteptăm la SAK = 0x08, conform datasheet-ului NXP.

*/
uint8_t Cititor_RFID::selecteazaCard_CL1(const uint8_t uidCL1[5], uint8_t &sak)
{

    sak = 0;
    if(uidCL1 == nullptr)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Pregatim comanda SELECT CL1: 93 70 UID0 UID1 UID2 UID3 BCC.
    uint8_t comanda[9] = {0x93, 0x70};

    for (uint8_t i = 0; i < 5; i++)
    {
        comanda[i+2] = uidCL1[i]; // completeaza de la bitul 3 in colo etc..;
    }
    
    // Calculam CRC_A pentru primii 7 octeti;
    if(!calculeazaCRC_A(comanda, 7, &comanda[7]))
        return 0;
    
    // Oprim comanda precedenta si pregatim FIFO;
    if(!scriereRegistru(0x01, 0x00) ||  // Idle
       !scriereRegistru(0x04, 0x7F) ||  // Stergem IRQ
       !scriereRegistru(0x0A, 0x80) ||  // Golim FIFO
       !scriereRegistru(0x0D, 0x00))    // Transmisie pe octeti intregi;
    {
        return 0;
    }

    // Introducem toti cei 9 octeti in FIFO;
    for(uint8_t i = 0; i < 9; i++)
    {
        if(!scriereRegistru(0x09, comanda[i]))
        {
            return 0;
        }
    }

    // Pornim transmisia si receptia;
    if(!scriereRegistru(0x01, 0x0C) || !scriereRegistru(0x0D, 0x80)) // in ordine Transceive, StartSend(1)
    {
        return 0;
    }

    uint8_t irq = 0;
    uint32_t timpStart = HAL_GetTick();

    // Asteptam raspunsul cardului, maxim 30 ms;
    while ((HAL_GetTick() - timpStart) < 30U)
    {
        if(!citireRegistru(0x04, irq))
            return 0;
        
        // RxIRq, ErrIRq sau TimerIRq;
        if((irq & 0x23) != 0)
            break;
    }

    // Citim starea inainte de a termina comanda;
    uint8_t erori   = 0;
    uint8_t lungime = 0;
    uint8_t control = 0;
    
    if(!citireRegistru(0x06, erori) || !citireRegistru(0x0A, lungime), !citireRegistru(0x0C, control))
    {
        return 0;
    }

    // Oprim StartSend si comanda Transceive;
    if (!scriereRegistru(0x0D, 0x00) || !scriereRegistru(0x01, 0x00))
    {
        return 0;
    }

    // Verificam daca am primit un raspuns.
    if ((irq & 0x20) == 0)
    {
        setEroare(EroareRFID::Timeout);
        return 0;
    }

    /*
      - 0xDF verifica toate erorile definite din ErrorReg, fara bitul reserved.
      - SAK trebuie sa contina exact 3 octeti: SAK + CRC_L + CRC_H.
     */
    if ((erori & 0xDF) != 0 || lungime != 3 || (control & 0x07) != 0)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Citim cei 3 octeti primiti;
    uint8_t raspuns[3] = {0};

    for (uint8_t i = 0; i < 3; i++)
    {
        if(!citireRegistru(0x09, raspuns[i]))
        {
            return 0;
        }
    }
    
    // Verificam CRC_A al octetului SAK;
    uint8_t crcCalculat[2] = {0};

    if(!calculeazaCRC_A(raspuns, 1, crcCalculat))
        return 0;
    
    if(crcCalculat[0] != raspuns[1] || crcCalculat[1] != raspuns[2])
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Raspunsul SELECT este valid;
    sak = raspuns[0];

    setEroare(EroareRFID::NicioEroare);
    return 1;

}

/* Conform datasheet-ului NXP, comanda MFAuthent = 0x0E primește în FIFO exact 12 octeți:
// [0x60] [0x04] [FF FF FF FF FF FF] [55 88 F0 06]
//   │      │          │                │
// Key A   Bloc     Cheia A          UID (4 octeti)


0x60 înseamnă autentificare cu Key A, iar 0x61 cu Key B conform datasheet;
Cheia [EXEMPLU] FF FF FF FF FF FF este cheia implicită întâlnită pe multe carduri MIFARE Classic nepersonalizate. 
Nu este garantat că toate cardurile goale o folosesc.
*/

uint8_t Cititor_RFID::autentificaBlocKeyA(uint8_t bloc, const uint8_t cheie[6], const uint8_t uid[4])
{
    if (cheie == nullptr || uid == nullptr)
    {
        setEroare(EroareRFID::AutentificareEsuata);
        return 0;
    }

    // 12 octeti necesari pentru MFAuthent;
    uint8_t date[12] = {0x60, bloc};

    // Copiem cheia de 6 octeti.
    for (uint8_t i = 0; i < 6; i++)
        date[i + 2] = cheie[i];

    // Copiem cei 4 octeti UID.
    for (uint8_t i = 0; i < 4; i++)
        date[i + 8] = uid[i];

    // Pregatim MFRC522 pentru autentificare;
    if (!scriereRegistru(0x01, 0x00) ||  // Idle
        !scriereRegistru(0x04, 0x7F) ||  // Sterge IRQ
        !scriereRegistru(0x0A, 0x80))    // Golim FIFO
        return 0;

    // Scriem in FIFO cei 12 octeti.
    for (uint8_t i = 0; i < 12; i++)
    {
        if (!scriereRegistru(0x09, date[i]))
            return 0;
    }

    // Pornim autentificarea hardware Crypto1;
    if (!scriereRegistru(0x01, 0x0E))
        return 0;

    uint8_t irq = 0;
    uint32_t timpStart = HAL_GetTick();

    // Asteptam finalizarea autentificarii;
    while ((HAL_GetTick() - timpStart) < 30U)
    {
        if (!citireRegistru(0x04, irq))
        {
            scriereRegistru(0x01, 0x00);
            return 0;
        }

        // IdleIRq, ErrIRq sau TimerIRq;
        if ((irq & 0x13) != 0)
            break;
    }

    // Oprim comanda daca nu s-a terminat deja;
    if (!scriereRegistru(0x01, 0x00))
        return 0;

    uint8_t status2 = 0;
    uint8_t erori = 0;

    if (!citireRegistru(0x08, status2) || !citireRegistru(0x06, erori))
        return 0;

    /*
     - Bitul 3 din Status2Reg este MFCrypto1 On;
     - Acesta devine 1 dupa autentificarea reusita;
    */
    if ((status2 & 0x08) == 0 || (erori & 0xDF) != 0)
    {
        setEroare(EroareRFID::AutentificareEsuata);
        return 0;
    }

    setEroare(EroareRFID::NicioEroare);
    return 1;
}

/*

    ***PENTRU TESTE, metoda finala va avea oprireCrypto1 -> dezactivareAntena -> [locked]activareAntena

*/
bool Cititor_RFID::resetareSesiuneRFID()
{

    uint8_t status2 = 0;
    uint8_t txControl = 0;

    // Oprim orice comanda activa;
    if (!scriereRegistru(0x01, 0x00))
        return false;

    // Citim Status2Reg;
    if (!citireRegistru(0x08, status2))
        return false;

    // Dezactivam Crypto1 prin stergerea bitului 3;
    status2 &= 0xF7;

    if (!scriereRegistru(0x08, status2))
        return false;

    // Citim registrul de control al antenei;
    if (!citireRegistru(0x14, txControl))
        return false;

    // Dezactivam TX1 si TX2, stergand bitii 0 si 1;
    txControl &= 0xFC;

    if (!scriereRegistru(0x14, txControl))
        return false;

    // Permitem descarcarea si resetarea cardului.
    HAL_Delay(50);

    // Reactivam antena folosind metoda existenta;
    if (!activareAntena())
        return false;

    // Asteptam stabilizarea campului RF;
    HAL_Delay(50);

    return true;
}


uint8_t Cititor_RFID::citesteBloc(uint8_t bloc, uint8_t date[16])
{
    // Verificam argumentele pentru MIFARE Classic 1K;
    if (date == nullptr || bloc > 63)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Verificam daca sesiunea Crypto1 este activa;
    uint8_t status2 = 0;

    if (!citireRegistru(0x08, status2))
        return 0;

    if ((status2 & 0x08) == 0)
    {
        setEroare(EroareRFID::AutentificareEsuata);
        return 0;
    }

    // Construim comanda READ: 0x30 + bloc + CRC_A;
    uint8_t comanda[4] = {0x30, bloc, 0, 0};

    // Calculam CRC_A pentru primii doi octeti;
    if (!calculeazaCRC_A(comanda, 2, &comanda[2]))
        return 0;

    // Pregatim registrele MFRC522 pentru transmisie;
    if (!scriereRegistru(0x01, 0x00) || // Idle
        !scriereRegistru(0x04, 0x7F) || // Sterge IRQ
        !scriereRegistru(0x0A, 0x80) || // Golim FIFO
        !scriereRegistru(0x0D, 0x00))   // Octeti intregi
    {
        return 0;
    }

    // Introducem comanda READ in FIFO;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (!scriereRegistru(0x09, comanda[i]))
        {
            return 0;
        }
    }

    // Pornim transmisia catre card;
    if (!scriereRegistru(0x01, 0x0C) || // Transceive
        !scriereRegistru(0x0D, 0x80))   // StartSend
    {
        return 0;
    }

    // Asteptam raspunsul cardului, maximum 30 ms;
    uint8_t irq = 0;
    uint32_t timpStart = HAL_GetTick();

    while ((HAL_GetTick() - timpStart) < 30U)
    {
        if (!citireRegistru(0x04, irq))
            return 0;

        // RxIRq, ErrIRq sau TimerIRq.
        if ((irq & 0x23) != 0)
            break;
    }

    // Citim informatiile despre raspuns;
    uint8_t erori = 0;
    uint8_t lungime = 0;
    uint8_t control = 0;

    if (!citireRegistru(0x06, erori)   ||
        !citireRegistru(0x0A, lungime) ||
        !citireRegistru(0x0C, control))
    {
        return 0;
    }

    // Incheiem comanda Transceive;
    if (!scriereRegistru(0x0D, 0x00) || !scriereRegistru(0x01, 0x00))
    {
        return 0;
    }

    // Verificam eventualele erori ale cititorului;
    if ((erori & 0xDF) != 0)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Verificam daca am primit raspuns;
    if ((irq & 0x20) == 0)
    {
        setEroare(EroareRFID::Timeout);
        return 0;
    }

    /*
     - Raspunsul trebuie sa contina 18 octeti:
      16 octeti de date + 2 octeti CRC_A;
    */
    if (lungime != 18 || (control & 0x07) != 0)
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Citim raspunsul complet din FIFO;
    uint8_t raspuns[18] = {0};

    for (uint8_t i = 0; i < 18; i++)
    {
        if (!citireRegistru(0x09, raspuns[i]))
        {
            return 0;
        }
    }

    // Calculam CRC_A pentru cei 16 octeti primiti;
    uint8_t crcCalculat[2] = {0};

    if (!calculeazaCRC_A(raspuns, 16, crcCalculat))
        return 0;

    // Comparam CRC-ul calculat cu cel primit;
    if (crcCalculat[0] != raspuns[16] || crcCalculat[1] != raspuns[17])
    {
        setEroare(EroareRFID::EroareCitire);
        return 0;
    }

    // Copiem datele numai dupa validarea raspunsului;
    for (uint8_t i = 0; i < 16; i++)
    {
        date[i] = raspuns[i];
    }

    setEroare(EroareRFID::NicioEroare);
    return 1;
}


/*
 - Transmite date prin MFRC522 si asteapta
 confirmarea ACK specifica MIFARE Classic.
 
 - Bufferul primit trebuie sa contina deja CRC_A.
*/
bool Cititor_RFID::transmite_cu_ack(const uint8_t *date, uint8_t lungime)
{
    // Pregatim registrele pentru transmisie;
    if (!scriereRegistru(0x01, 0x00) || // Idle
        !scriereRegistru(0x04, 0x7F) || // Stergem IRQ
        !scriereRegistru(0x0A, 0x80) || // Golim FIFO
        !scriereRegistru(0x0D, 0x00))   // 8 biti/octet
    {
        return false;
    }

    // Introducem datele in FIFO;
    for (uint8_t i = 0; i < lungime; i++)
    {
        if (!scriereRegistru(0x09, date[i]))
        {
            return false;
        }
    }

    // Pornim transmisia radio;
    if (!scriereRegistru(0x01, 0x0C) || // Transceive
        !scriereRegistru(0x0D, 0x80))   // StartSend
    {
        return false;
    }

    uint8_t irq = 0;
    uint32_t timpStart = HAL_GetTick();

    // Asteptam confirmarea cardului, maximum 30 ms;
    while ((HAL_GetTick() - timpStart) < 30U)
    {
        if (!citireRegistru(0x04, irq))
        {
            scriereRegistru(0x01, 0x00);
            return false;
        }

        // RxIRq, ErrIRq sau TimerIRq;
        if ((irq & 0x23) != 0)
        {
            break;
        }
    }

    // Citim informatiile despre raspuns;
    uint8_t erori = 0;
    uint8_t nrOcteti = 0;
    uint8_t control = 0;

    if (!citireRegistru(0x06, erori)    ||
        !citireRegistru(0x0A, nrOcteti) ||
        !citireRegistru(0x0C, control))
    {
        return false;
    }

    // Incheiem comanda Transceive;
    if (!scriereRegistru(0x0D, 0x00) ||
        !scriereRegistru(0x01, 0x00))
    {
        return false;
    }

    // Verificam erorile, exceptand CRCErr;
    // Raspunsul ACK nu contine CRC;
    if ((erori & 0xDB) != 0)
    {
        setEroare(EroareRFID::EroareScriere);
        return false;
    }

    // Verificam daca am primit un raspuns;
    if ((irq & 0x20) == 0)
    {
        setEroare(EroareRFID::Timeout);
        return false;
    }

    // ACK ocupa un octet FIFO, dar are doar 4 biti valizi;
    if (nrOcteti != 1 || (control & 0x07) != 4)
    {
        setEroare(EroareRFID::EroareScriere);
        return false;
    }

    // Citim raspunsul din FIFO;
    uint8_t ack = 0;

    if (!citireRegistru(0x09, ack))
    {
        return false;
    }

    /* ACK:       xxxx 1010
       Masca:     0000 1111
                  ---------
       Rezultat:  0000 1010 = 0x0A
     - Conform datasheet-ului MIFARE Classic, secțiunea 9.3, valoarea 0x0A reprezintă confirmarea pozitivă ACK;
    */
    // Verificam cei 4 biti semnificativi ai raspunsului;
    if ((ack & 0x0F) != 0x0A)
    {
        setEroare(EroareRFID::EroareScriere);
        return false;
    }

    setEroare(EroareRFID::NicioEroare);
    return true;
}


/*  

  - Spre deosebire de citire, scrierea se realizează în două etape, conform datasheet-ului NXP, secțiunea 12.3, pagina 22. 
  - Scriem un bloc de date MIFARE Classic 1K folosind procedura WRITE in doua etape.

*/
uint8_t Cititor_RFID::scrieBloc(uint8_t bloc, const uint8_t date[16])
{

    /*
        - Verificam argumentele;
        - Protejam temporar MANUFACTURER BLOACK (0) si toate blcurile SECTOR TRAILER;
    */

    // verifcam ca data sa nu fie null , bloc nu mai mare de 63, nu egal cu 0 iar bloc % 4 == 3 sa nu scriem in SECTOR TRAILER
    if (date == nullptr || bloc > 63 || bloc == 0 || (bloc % 4) == 3)
    {
        setEroare(EroareRFID::EroareScriere);
        return 0;
    }
    
    // Verificam daca sesiunea Crypto1 este activa;
    uint8_t status2 = 0;

    if(!citireRegistru(0x08, status2))
        return 0;
    if((status2 && 0x08) == 0)
    {
        setEroare(EroareRFID::AutentificareEsuata);
        return 0;
    }

    /*
        - ETAPA 1:
        Pregatim comanda WRITE si adresa blocului;
    */
    uint8_t comanda[4] = {0xA0, bloc, 0, 0};
    if(!calculeazaCRC_A(comanda, 2, &comanda[2]))
        return 0;
    
    /*
        - Pregatim pentru ETAPA 2:
        -> 16 octeti de date + 2 octeti CRC_A
    */
    uint8_t buffer[18] = {0};
    for(uint8_t i = 0; i < 16; i++)
    {
        buffer[i] = date[i];
    }

    if(!calculeazaCRC_A(buffer, 16, &buffer[16]))
        return 0;

    // Transmitem comanda WRITE si verificam primul ACK;
    if(!transmite_cu_ack(comanda, 4))
        return 0;
    
    // Transmitem datele si verificam al doilea ACK;
    if(!transmite_cu_ack(buffer, 18))
        return 0;
    
    // Ambele etape au fost confirmate de card cu succes;
    setEroare(EroareRFID::NicioEroare);
    return 1;

}
