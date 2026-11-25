#include <Memorie_RFID.h>

Memorie_RFID::Memorie_RFID()
{



}

// Seteaza eroarea;
void Memorie_RFID::setEroare(RFID_MEM_ERR eroare)
{
    ultimaEroare = eroare;
}

/*
 - Verifica daca fiecare locatie de 32 de biti din pagina Flash daca contine valoarea 0xFFFFFFFF;
 - Aceasta functie doar CITESTE memoria, nu face alte operatii;
*/
uint8_t Memorie_RFID::paginaEsteGoala() const
{
    // Parcurgem intreaga pagina din 4 in 4 octeti, adica din 32 in 32 de biti;
    for (uint32_t offset = 0; offset < this->DIMENSIUNE_PAGINA; offset += 4)
    {
        // Obtinem adresa locatiei curente;
        uint32_t adresa = this->ADRESA_FLASH + offset;

        /* 
            
            *** Citim continutul Flash direct din memorie a pagini 128[ULTIMA-PAGINA] ***

           - reinterpret_cast transforma adresa numerica intrun pointer fara nicio conversia a valori, * ia valoarea 
           efectiva si o atribuie lui valoare, iar volatile indica regulile de acces la memorie;

           - volatile ii spune compilatorului că accesul la acea locație de memorie trebuie 
           efectuat efectiv(real) și nu trebuie eliminat sau combinat arbitrar cu alte accesări volatile, mai 
           pe scurt ii spune compilatorului sa nu o optimizeze aceasta variabila etc.

           - const asigura ca doar citim prin acest pointer, si in niciun caz scriiere;
        */
        uint32_t valoare = *reinterpret_cast<volatile const uint32_t*>(adresa);
        //        val    = val_ef(adr_numerica(adresa) -> pointer de typ uint32_t);

        // O valoare diferita de 0xFFFFFFFF indica date.
        if (valoare != 0xFFFFFFFFU)
        {
            return 0;
        }

    }

    // Toata pagina este stearsa, se returneaza 1;
    return 1;
}

uint8_t Memorie_RFID::stergePagina128()
{
    // Resetam erorile ramase de la operatiile anterioare;
    setEroare(RFID_MEM_ERR::NicioEroare);
    memoryError.halFlash_ErrorHandle(HAL_OK);

    /*
     - Verificam daca pagina este deja goala;
     - Evitam un ciclu inutil de stergere Flash;
    */
    if (paginaEsteGoala())
    {
        return 1;
    }

    // Adresa paginii 128 (index 127);
    const uint32_t adresaPagina = ADRESA_FLASH;

    // Structura de configurare a operatiei de stergere;
    FLASH_EraseInitTypeDef configurareStergere = {0};

    // Selectam tipul operatiei de stergere;
    // FLASH_TYPEERASE_PAGES permite stergerea individuala
    // a uneia sau a mai multor pagini, fara Mass Erase;
    configurareStergere.TypeErase = FLASH_TYPEERASE_PAGES;

    // Selectam primul banc al memoriei Flash;
    // STM32F103CBT6 dispune de un singur banc Flash;
    // Acest parametru este relevant in special pentru Mass Erase;
    configurareStergere.Banks = FLASH_BANK_1;

    // Specificam adresa de inceput a paginii care va fi stearsa;
    // 0x0801FC00 reprezinta pagina 128 (index 127),
    // rezervata exclusiv pentru stocarea datelor RFID;
    configurareStergere.PageAddress = adresaPagina;

    // Specificam numarul de pagini consecutive care vor fi sterse,
    // incepand de la adresa definita prin PageAddress;
    // O pagina are dimensiunea de 1024 octeti;
    // Valoarea 1 asigura stergerea unei singure pagini;
    configurareStergere.NbPages = 1;

    // Adresa paginii la care a aparut o eventuala eroare;
    uint32_t pageError = 0xFFFFFFFFU;

    // Deblocam controlerul Flash;
    HAL_StatusTypeDef status = HAL_FLASH_Unlock();

    if (status != HAL_OK)
    {
        memoryError.halFlash_ErrorHandle(status, false);

        setEroare(RFID_MEM_ERR::DeblocareEsuata);
        return 0;
    }

    // Executam stergerea paginii;
    status = HAL_FLASHEx_Erase(&configurareStergere, &pageError);

    // Verificam rezultatul operatiei de stergere;
    if (status != HAL_OK)
    {
        // Interpretam eroarea hardware prin clasa comuna;
        memoryError.halFlash_ErrorHandle(status);

        // Retinem eroarea specifica modulului RFID;
        setEroare(RFID_MEM_ERR::StergereEsuata);

        // Blocam controlerul chiar daca stergerea a esuat;
        HAL_FLASH_Lock();

        return 0;
    }

    // Blocam controlerul Flash;
    status = HAL_FLASH_Lock();

    if (status != HAL_OK)
    {
        memoryError.halFlash_ErrorHandle(status, false);

        setEroare(RFID_MEM_ERR::BlocareEsuata);
        return 0;
    }

    /*
     - Verificam fizic continutul intregii pagini;
     - Functia returneaza 1 daca toti octetii sunt 0xFF;
    */
    if (!paginaEsteGoala())
    {
        setEroare(RFID_MEM_ERR::VerificareStergereEsuata);
        return 0;
    }

    // Toate operatiile au fost executate cu succes;
    setEroare(RFID_MEM_ERR::NicioEroare);

    return 1;
}
