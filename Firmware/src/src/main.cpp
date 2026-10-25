#include <stdint.h>

#define RCC_APB2ENR   (*(volatile uint32_t*)0x40021018)

#define GPIOC_CRH     (*(volatile uint32_t*)0x40011004)
#define GPIOC_BSRR    (*(volatile uint32_t*)0x40011010)
#define GPIOC_BRR     (*(volatile uint32_t*)0x40011014)

#define LED_PIN 13

static void delay()
{
    for (volatile uint32_t i = 0; i < 100000; i++)
    {
        // becul albastru de pe board licare la fiecare vreo juma de minut*
    }
}

int main()
{
    // 1. Enable clock pentru GPIOC
    // RCC_APB2ENR bit 4 = IOPCEN
    RCC_APB2ENR |= (1 << 4);
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);

    while (1)
    {
        // PC13 LOW -> LED ON
        GPIOC_BRR = (1 << LED_PIN);

        delay();

        // PC13 HIGH -> LED OFF
        GPIOC_BSRR = (1 << LED_PIN);

        delay();
    }
}