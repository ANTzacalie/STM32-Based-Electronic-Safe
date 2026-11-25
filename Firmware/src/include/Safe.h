#ifndef SAFE_H
#define SAFE_H

#include "stm32f1xx_hal.h"
#include <memory_handleError.h>
#include <stdint.h>


enum class EroareSeif : uint8_t
{

    // DE ADAUGA ERORILE GENERALE ALE SEIFULUI;

};


class Safe {

    public:

        Safe();

    private:
    
        // Gestionarea erorilor HAL Flash;
        memory_handleError memoryError;

    protected:

};





#endif