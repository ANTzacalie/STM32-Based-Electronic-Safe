#include <stdint.h>

#define RCC_APB2ENR   (*(volatile uint32_t*)0x40021018)

#define GPIOC_CRH     (*(volatile uint32_t*)0x40011004)
#define GPIOC_BSRR    (*(volatile uint32_t*)0x40011010)
#define GPIOC_BRR     (*(volatile uint32_t*)0x40011014)

#define LED_PIN 13

static void delay()
{
    for (volatile uint32_t i = 0; i < 5000000; i++)
    {
        // becul albastru de pe board licare la fiecare vreo juma de minut
    }
}

int main()
{
    // 1. Enable clock pentru GPIOC
    // RCC_APB2ENR bit 4 = IOPCEN
    RCC_APB2ENR |= (1 << 4);

    // 2. Configure PC13 ca General Purpose Output Push-Pull
    //
    // PC13 este în CRH.
    // Fiecare pin are 4 biți:
    //
    // CNF13  = 00 -> General purpose push-pull
    // MODE13 = 10 -> Output max 2 MHz
    //
    // PC13 => bits 23:20

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