

* All about the APM32F103CBT6 / STM32F103CB Development Board*

 ---------------------------
| Peripheral | Base Address |
|------------|--------------|
| RCC        | `0x40021000` |
| FLASH      | `0x40022000` |
| GPIOA      | `0x40010800` |
| GPIOB      | `0x40010C00` |
| GPIOC      | `0x40011000` |
| AFIO       | `0x40010000` |
| EXTI       | `0x40010400` |
| NVIC (core)| `0xE000E100` |
| TIM2       | `0x40000000` |
| TIM3       | `0x40000400` |
| TIM4       | `0x40000800` |
| TIM1       | `0x40012C00` |
| SPI1       | `0x40013000` |
| SPI2       | `0x40003800` |
| I2C1       | `0x40005400` |
| I2C2       | `0x40005800` |
| USART1     | `0x40013800` |
| USART2     | `0x40004400` |
| USART3     | `0x40004800` |
| ADC1       | `0x40012400` |
| ADC2       | `0x40012800` |
| PWR        | `0x40007000` |
| DMA1       | `0x40020000` |
 ---------------------------

 ----------------------------------------------------------------------------------------------------------------------------------------- 
| Peripheral |            Register(s)           |                                       Function                                           |
|------------|----------------------------------|------------------------------------------------------------------------------------------|
| RCC        | `CR`                             | Oscillator control: HSI, HSE, and PLL enable; ready flags.                               |
| RCC        | `CFGR`                           | SYSCLK source selection, AHB/APB1/APB2 prescalers, PLL source and multiplication factor. |
| RCC        | `CIR`                            | Clock interrupt status and flag clearing.                                                |
| RCC        | `APB2ENR`                        | Clock enable for GPIOA–E, AFIO, USART1, SPI1, TIM1, and ADC1–2.                          |
| RCC        | `APB1ENR`                        | Clock enable for TIM2–4, SPI2, I2C1–2, USART2–3, and PWR.                                |
| RCC        | `AHBENR`                         | Clock enable for DMA1, the Flash interface (FLITF), and the SRAM interface.              |
| RCC        | `BDCR`                           | Backup domain control: LSE and RTC clock source selection.                               |
| RCC        | `CSR`                            | Reset cause flags: watchdog, reset pin, and power reset.                                 |
| FLASH      | `ACR`                            | Flash wait states and prefetch buffer configuration.                                     |
| FLASH      | `KEYR / OPTKEYR`                 | Keys for unlocking Flash memory and option byte programming.                             |
| FLASH      | `SR / CR`                        | Flash erase/program status and control.                                                  |
| FLASH      | `OBR / WRPR`                     | Option byte readback and write protection information.                                   |
| GPIOx      | `CRL / CRH`                      | Pin mode and output speed configuration for pins 0–7 / 8–15.                             |
| GPIOx      | `IDR`                            | Read input pin states.                                                                   |
| GPIOx      | `ODR`                            | Output data register: read or write output pin values.                                   |
| GPIOx      | `BSRR / BRR`                     | Atomic setting and resetting of individual output bits.                                  |
| GPIOx      | `LCKR`                           | Lock pin configuration.                                                                  |
| AFIO       | `EVCR`                           | External event pin configuration.                                                        |
| AFIO       | `MAPR / MAPR2`                   | Remap peripheral functions to alternative pins.                                          |
| AFIO       | `EXTICR1–4`                      | Select the source GPIO port for each EXTI line.                                          |
| EXTI       | `IMR / EMR`                      | Interrupt and event masks for each line.                                                 |
| EXTI       | `RTSR / FTSR`                    | Rising-edge and falling-edge trigger selection.                                          |
| EXTI       | `SWIER`                          | Software triggering of EXTI lines.                                                       |
| EXTI       | `PR`                             | Pending flags; cleared by writing `1` to the corresponding bit.                          |
| NVIC       | `ISER / ICER`                    | Enable or disable individual interrupts.                                                 |
| NVIC       | `ISPR / ICPR`                    | Set or clear interrupt pending flags.                                                    |
| NVIC       | `IPR`                            | Priority configuration for each interrupt.                                               |
| SCB        | `AIRCR`                          | Interrupt priority grouping: preemption priority and subpriority.                        |
| TIMx       | `CR1 / CR2`                      | Counter mode and trigger output configuration.                                           |
| TIMx       | `SMCR`                           | Slave mode configuration and synchronization with another timer.                         |
| TIMx       | `DIER / SR`                      | Interrupt/DMA request enable and status flags.                                           |
| TIMx       | `EGR`                            | Software event generation.                                                               |
| TIMx       | `CCMR1 / CCMR2`                  | PWM, input capture, and output compare mode configuration.                               |
| TIMx       | `CCER`                           | Channel output enable and polarity configuration.                                        |
| TIMx       | `CNT / PSC / ARR`                | Counter value, prescaler, and auto-reload value.                                         |
| TIMx       | `CCR1–4`                         | Capture/compare values for each channel; used to set PWM duty cycle.                     |
| TIM1       | `RCR / BDTR`                     | Repetition counter and break/dead-time configuration.                                    |
| SPIx       | `CR1 / CR2`                      | Master/slave mode, clock polarity and phase, and peripheral enable.                      |
| SPIx       | `SR`                             | Status flags: TXE, RXNE, BSY, and errors.                                                |
| SPIx       | `DR`                             | Transmit/receive data register.                                                          |
| SPIx       | `CRCPR`                          | CRC polynomial configuration.                                                            |
| I2Cx       | `CR1 / CR2`                      | START, STOP, and acknowledgment control; peripheral clock frequency configuration.       |
| I2Cx       | `OAR1 / OAR2`                    | Own address configuration for slave mode.                                                |
| I2Cx       | `DR`                             | Data register.                                                                           |
| I2Cx       | `SR1 / SR2`                      | Status flags: START condition, address match, and errors.                                |
| I2Cx       | `CCR / TRISE`                    | Bus speed configuration for Standard/Fast mode and maximum rise-time setting.            |
| USARTx     | `SR`                             | Status flags: transmission complete, receive data register not empty, and errors.        |
| USARTx     | `DR`                             | Transmit/receive data register.                                                          |
| USARTx     | `BRR`                            | Baud rate configuration.                                                                 |
| USARTx     | `CR1 / CR2 / CR3`                | Communication configuration: parity, stop bits, and flow control.                        |
| ADCx       | `SR / CR1 / CR2`                 | Conversion status and control.                                                           |
| ADCx       | `SMPR1 / SMPR2`                  | Sampling time configuration for each channel.                                            |
| ADCx       | `SQR1–3`                         | Regular conversion channel sequence configuration.                                       |
| ADCx       | `DR`                             | Conversion result.                                                                       |
| PWR        | `CR / CSR`                       | Low-power mode control and wake-up status.                                               |
| DMA1       | `ISR / IFCR`                     | Transfer status flags and flag clearing for each channel.                                |
| DMA1       | `CCRx / CNDTRx / CPARx / CMARx`  | Channel configuration, number of transfers, peripheral address, and memory address.      |
 ------------------------------------------------------------------------------------------------------------------------------------------