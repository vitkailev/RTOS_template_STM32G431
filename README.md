# FreeRTOS template for STM32G431RB

Project template for STM32G431RB microcontroller (native FreeRTOS).  
Hardware: [board](https://www.st.com/en/evaluation-tools/nucleo-g431rb.html)  
Library: [STM32 HAL](https://github.com/STMicroelectronics/STM32CubeG4)

## Features

- [x] Button;
- [x] LED;
- [x] 2 analog input pins;
- [x] Build-in temperature sensor;
- [x] PWM;
- [x] MCU-to-PC UART connection;
- [x] I2C interface;
- [x] CRC-32/ISO-HDLC;
- [x] Independent WDT;

## MCU Settings

1) Clock: HSI(16MHz) -> PLL -> SYSCLK(144MHz) -> HCLK(144MHz) -> PCLK1(36MHz) / PCLK2(36MHz);
2) Digital input pins: PC13 (pull-down);
3) Digital output pins: PA5 (pull-down);
4) SystemTick timer: 1kHz;
5) General Purpose Timer 15: 100kHz;
6) ADC: 12-bits, right, 247.5 cycles, DMA, Timer 15, PA0/PA1 + temperature sensor;
7) PWM: Timer 16, 10kHz, channel 1, complementary, PB4/PB6, dead time - 24 (330 ns);
8) UART: UART1 - PC4/PC5, 115200, 8N1, TX/RX-IDLE, FIFO - disabled, DMA;
9) I2C: I2C1 - PB8/PB9, 400 kHz, 7 bits address, DMA;
10) CRC: input data - bytes, polynomial - 0x04C11DB7, init value - 0xFFFFFFFF, input inversion, output inversion;
11) WDT: 1 second, prescaler - 8, reload value = 0x0FFF;

## Project structure

| Folder name | Description                                             |
|:-----------:|:--------------------------------------------------------|
|     app     | Source code                                             |
|    core     | CMSIS and core ARM/Cortex header files                  |
|   lib/hal   | Hardware abstraction layer(HAL) source and header files |
|    rtos     | FreeRTOS source and header files                        |
|   startup   | Linker files                                            |
|   system    | System source and header files                          |

## Project settings

- CMakeLists.txt file
