#include <Memorie_RFID.h>

Memorie_RFID::Memorie_RFID()
{



}

/*
 - Verifica daca fiecare locatie de 32 de biti din pagina Flash contine valoarea 0xFFFFFFFF.
 - Aceasta functie doar CITESTE memoria.
*/
uint8_t Memorie_RFID::paginaEsteGoala() const
{
    // Parcurgem intreaga pagina din 4 in 4 octeti.
    for (uint32_t offset = 0; offset < this->DIMENSIUNE_PAGINA; offset += 4)
    {
        // Obtinem adresa locatiei curente;
        uint32_t adresa = this->ADRESA_FLASH + offset;

        /* Citim continutul Flash direct din memorie;

           - reinterpret_cast transforma adresa numerica intrun pointer, * ia valoarea 
           efectiva, iar volatile indica tipul si regulile de acces la memorie;

           - volatile îi spune compilatorului că accesul la acea locație de memorie trebuie 
           efectuat efectiv și nu trebuie eliminat sau combinat arbitrar cu alte accesări volatile.

           
        */
        uint32_t valoare = *reinterpret_cast<volatile const uint32_t*>(adresa);

        // O valoare diferita de 0xFFFFFFFF indica date.
        if (valoare != 0xFFFFFFFFU)
        {
            return 0;
        }
    }

    // Toata pagina este stearsa.
    return 1;
}