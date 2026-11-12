
#include "../include/Cititor_RFID.h"


// Se executa utomat la creearea unui obiect de tip Cititor_RFID
Cititor_RFID::Cititor_RFID() 
{

    // conf. pini necesari mfrc522
    initializarePini();

    // configurare periferic SPI1
    initializareSPI();

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

    // Aplicam configuratia si verificam rezultatul;
    if(HAL_SPI_Init(&hspi1) != HAL_OK) // daca HAL_SPI_Init nu intoarce HAL_OK la configuratia hspi1, inseamna ca SPI1 a esuat;
    {
        setEroare(EroareRFID::InitializareEsuata);
    }

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