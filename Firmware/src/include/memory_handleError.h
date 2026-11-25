#ifndef MEMORY_HANDLE_ERROR_H
#define MEMORY_HANDLE_ERROR_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

// Erorile comune operatiilor HAL Flash;
enum class FLASH_MEM_ERR : uint8_t
{
    
    NicioEroare = 0,
    FlashOcupat,
    FlashProtejat,
    Timeout,
    EroareProgramare,
    EroareOptionBytes,
    OperatieEsuata,
    EroareNecunoscuta

};

class memory_handleError
{

    private:
        // Ultima eroare interpretata;
        FLASH_MEM_ERR ultimaEroare = FLASH_MEM_ERR::NicioEroare;

        // Masca originala de erori furnizata de HAL;
        uint32_t ultimaEroareHAL = HAL_FLASH_ERROR_NONE;

    public:
        // Interpreteaza rezultatul unei operatii HAL Flash;
        void halFlash_ErrorHandle(HAL_StatusTypeDef status, bool citesteMascaHAL = true);

        // Returneaza eroarea interpretata;
        FLASH_MEM_ERR getUltimaEroare() const;

        // Returneaza masca originala de erori HAL;
        uint32_t getUltimaEroareHAL() const;

};

#endif
