
#include <memory_handleError.h>

/*
  - Interpreteaza statusul HAL si daca este cazul, masca de erori a controlerului Flash.
*/
void memory_handleError::halFlash_ErrorHandle(HAL_StatusTypeDef status, bool citesteMascaHAL)
{

    // Resetam informatiile despre eroarea anterioara;
    ultimaEroare = FLASH_MEM_ERR::NicioEroare;
    ultimaEroareHAL = HAL_FLASH_ERROR_NONE;

    // Interpretam statusul general returnat de HAL;
    switch (status)
    {
        case HAL_OK:
            return;

        case HAL_BUSY:
            ultimaEroare = FLASH_MEM_ERR::FlashOcupat;
            return;

        case HAL_TIMEOUT:
            ultimaEroare = FLASH_MEM_ERR::Timeout;
            return;

        case HAL_ERROR:
            break;

        default:
            ultimaEroare = FLASH_MEM_ERR::EroareNecunoscuta;
            return;
    }

    // Citim masca HAL numai daca operatia o justifica;
    if (citesteMascaHAL)
    {
        ultimaEroareHAL = HAL_FLASH_GetError();
    }

    // Verificam protectia la scriere;
    if (ultimaEroareHAL & HAL_FLASH_ERROR_WRP)
    {
        ultimaEroare = FLASH_MEM_ERR::FlashProtejat;
    }
    // Verificam validitatea Option Bytes;
    else if (ultimaEroareHAL & HAL_FLASH_ERROR_OPTV)
    {
        ultimaEroare = FLASH_MEM_ERR::EroareOptionBytes;
    }
    // Verificam eroarea de programare;
    else if (ultimaEroareHAL & HAL_FLASH_ERROR_PROG)
    {
        ultimaEroare = FLASH_MEM_ERR::EroareProgramare;
    }
    // HAL a raportat esec, fara o eroare hardware cunoscuta;
    else
    {
        ultimaEroare = FLASH_MEM_ERR::OperatieEsuata;
    }

}

// Returnam ultima eroare interpretata;
FLASH_MEM_ERR memory_handleError::getUltimaEroare() const
{
    return ultimaEroare;
}

// Returnam masca originala de erori HAL;
uint32_t memory_handleError::getUltimaEroareHAL() const
{
    return ultimaEroareHAL;
}
