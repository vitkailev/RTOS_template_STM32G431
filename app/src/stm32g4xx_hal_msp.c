/**
  ******************************************************************************
  * @file    stm32g4xx_hal_msp_template.c
  * @author  MCD Application Team
  * @brief   HAL MSP module.
  *          This file template is located in the HAL folder and should be copied
  *          to the user folder.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/** @addtogroup STM32G4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP module driver
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static DMA_HandleTypeDef dma1Handle;
static DMA_HandleTypeDef dma2Handle;
static DMA_HandleTypeDef dma3Handle;
static DMA_HandleTypeDef dma4Handle;
static DMA_HandleTypeDef dma5Handle;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initialize the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void) {
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_PWREx_DisableUCPDDeadBattery();
}

/**
  * @brief  DeInitialize the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspDeInit(void) {
}

/**
 * @brief Initialize the base timers, turn ON a clock source and setup interrupt vector
 * @param htim is the pointer to the data structure of the base timer handle (HAL).
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM15) {
        __HAL_RCC_TIM15_CLK_ENABLE();
    }
}

/**
 * @brief DeInitialize the base timers
 * @param htim is the pointer to the data structure of the base timer handle (HAL).
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM15) {
        __HAL_RCC_TIM15_FORCE_RESET();
        __HAL_RCC_TIM15_RELEASE_RESET();
        __HAL_RCC_TIM15_CLK_DISABLE();
    }
}

/**
 * @brief Initialize the PWM timer mode, turn ON a clock source and setup GPIOs
 * @param htim is the pointer to the data structure of the timer handle (HAL).
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef gpioInit = {0};

    if (htim->Instance == TIM16) {
        __HAL_RCC_TIM16_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        gpioInit.Pin = GPIO_PIN_4 | GPIO_PIN_6;
        gpioInit.Mode = GPIO_MODE_AF_PP;
        gpioInit.Pull = GPIO_PULLDOWN;
        gpioInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpioInit.Alternate = GPIO_AF1_TIM16;
        HAL_GPIO_Init(GPIOB, &gpioInit);
    }
}

/**
 * @brief DeInitialize the PWM timer mode
 * @param htim is the pointer to the data structure of the timer handle (HAL).
 */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM16) {
        __HAL_RCC_TIM16_FORCE_RESET();
        __HAL_RCC_TIM16_RELEASE_RESET();
        __HAL_RCC_TIM16_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
    }
}

/**
 * @brief Initialize the ADC module, turn ON a clock source, setup GPIO and interrupt vector
 * @param hadc is the pointer to the data structure of the ADC handle (HAL).
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc) {
    GPIO_InitTypeDef gpioInit = {0};
    RCC_PeriphCLKInitTypeDef clockInit = {0};

    if (hadc->Instance == ADC1) {
        clockInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
        clockInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_PLL;

        if (HAL_RCCEx_PeriphCLKConfig(&clockInit) == HAL_OK) {
            __HAL_RCC_ADC12_CLK_ENABLE();

            __HAL_RCC_GPIOA_CLK_ENABLE();
            gpioInit.Pin = GPIO_PIN_0 | GPIO_PIN_1;
            gpioInit.Mode = GPIO_MODE_ANALOG;
            gpioInit.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(GPIOA, &gpioInit);

            __HAL_RCC_DMAMUX1_CLK_ENABLE();
            __HAL_RCC_DMA1_CLK_ENABLE();

            DMA_HandleTypeDef *dmaInit = &dma1Handle;
            dmaInit->Instance = DMA1_Channel1;
            dmaInit->Init.Request = DMA_REQUEST_ADC1;
            dmaInit->Init.Direction = DMA_PERIPH_TO_MEMORY;
            dmaInit->Init.PeriphInc = DMA_PINC_DISABLE;
            dmaInit->Init.MemInc = DMA_MINC_ENABLE;
            dmaInit->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            dmaInit->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
            dmaInit->Init.Mode = DMA_CIRCULAR;
            dmaInit->Init.Priority = DMA_PRIORITY_HIGH;
            if (HAL_DMA_Init(dmaInit) == HAL_OK) {
                __HAL_LINKDMA(hadc, DMA_Handle, *dmaInit);

                HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
                HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
            }
        }
    }
}

/**
 * @brief DeInitialize the ADC module
 * @param hadc is the pointer to the data structure of the ADC handle (HAL).
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC12_FORCE_RESET();
        __HAL_RCC_ADC12_RELEASE_RESET();
        __HAL_RCC_ADC12_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

        HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
        HAL_DMA_DeInit(&dma1Handle);
    }
}

/**
 * @brief Initialize the UART interfaces, turn ON a clock source, setup GPIO and interrupt vector
 * @param huart is the pointer to the data structure of the UART handle (HAL).
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    GPIO_InitTypeDef gpioInit = {0};
    RCC_PeriphCLKInitTypeDef clockInit = {0};
    DMA_HandleTypeDef *dmaInit = NULL;

    if (huart->Instance == USART1) {
        clockInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
        clockInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;

        if (HAL_RCCEx_PeriphCLKConfig(&clockInit) == HAL_OK) {
            __HAL_RCC_USART1_CLK_ENABLE();

            __HAL_RCC_GPIOC_CLK_ENABLE();
            gpioInit.Pin = GPIO_PIN_4 | GPIO_PIN_5;
            gpioInit.Mode = GPIO_MODE_AF_PP;
            gpioInit.Pull = GPIO_PULLUP;
            gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
            gpioInit.Alternate = GPIO_AF7_USART1;
            HAL_GPIO_Init(GPIOC, &gpioInit);

            __HAL_RCC_DMAMUX1_CLK_ENABLE();
            __HAL_RCC_DMA1_CLK_ENABLE();

            dmaInit = &dma2Handle;
            dmaInit->Instance = DMA1_Channel4;
            dmaInit->Init.Request = DMA_REQUEST_USART1_TX;
            dmaInit->Init.Direction = DMA_MEMORY_TO_PERIPH;
            dmaInit->Init.PeriphInc = DMA_PINC_DISABLE;
            dmaInit->Init.MemInc = DMA_MINC_ENABLE;
            dmaInit->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            dmaInit->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            dmaInit->Init.Mode = DMA_NORMAL;
            dmaInit->Init.Priority = DMA_PRIORITY_LOW;

            if (HAL_DMA_Init(dmaInit) == HAL_OK) {
                __HAL_LINKDMA(huart, hdmatx, *dmaInit);

                HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 7, 0);
                HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
            }

            dmaInit = &dma3Handle;
            dmaInit->Instance = DMA1_Channel5;
            dmaInit->Init.Request = DMA_REQUEST_USART1_RX;
            dmaInit->Init.Direction = DMA_PERIPH_TO_MEMORY;
            dmaInit->Init.PeriphInc = DMA_PINC_DISABLE;
            dmaInit->Init.MemInc = DMA_MINC_ENABLE;
            dmaInit->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            dmaInit->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            dmaInit->Init.Mode = DMA_NORMAL;
            dmaInit->Init.Priority = DMA_PRIORITY_HIGH;
            if (HAL_DMA_Init(dmaInit) == HAL_OK) {
                __HAL_LINKDMA(huart, hdmarx, *dmaInit);

                HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 6, 0);
                HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
            }

            HAL_NVIC_SetPriority(USART1_IRQn, 8, 0);
            HAL_NVIC_EnableIRQ(USART1_IRQn);
        }
    }
}

/**
 * @brief DeInitialize the UART interfaces
 * @param huart is the pointer to the data structure of the UART handle (HAL).
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        __HAL_RCC_USART1_FORCE_RESET();
        __HAL_RCC_USART1_RELEASE_RESET();
        __HAL_RCC_USART1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_4);
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_5);

        HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);
        HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);
        HAL_NVIC_DisableIRQ(USART1_IRQn);

        HAL_DMA_DeInit(&dma2Handle);
        HAL_DMA_DeInit(&dma3Handle);
    }
}

/**
 * @brief Initialize the I2C interfaces, turn ON a clock source, setup GPIO and interrupt vector
 * @param hi2c is the pointer to the data structure of the I2C handle (HAL)
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    GPIO_InitTypeDef gpioInit = {0};
    RCC_PeriphCLKInitTypeDef clockInit = {0};
    DMA_HandleTypeDef *dmaInit = NULL;

    if (hi2c->Instance == I2C1) {
        clockInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
        clockInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;

        if (HAL_RCCEx_PeriphCLKConfig(&clockInit) == HAL_OK) {
            __HAL_RCC_I2C1_CLK_ENABLE();

            __HAL_RCC_GPIOB_CLK_ENABLE();
            gpioInit.Pin = GPIO_PIN_8 | GPIO_PIN_9;
            gpioInit.Mode = GPIO_MODE_AF_OD;
            gpioInit.Pull = GPIO_PULLUP;
            gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
            gpioInit.Alternate = GPIO_AF4_I2C1;
            HAL_GPIO_Init(GPIOB, &gpioInit);

            __HAL_RCC_DMAMUX1_CLK_ENABLE();
            __HAL_RCC_DMA1_CLK_ENABLE();

            dmaInit = &dma4Handle;
            dmaInit->Instance = DMA1_Channel2;
            dmaInit->Init.Request = DMA_REQUEST_I2C1_TX;
            dmaInit->Init.Direction = DMA_MEMORY_TO_PERIPH;
            dmaInit->Init.PeriphInc = DMA_PINC_DISABLE;
            dmaInit->Init.MemInc = DMA_MINC_ENABLE;
            dmaInit->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            dmaInit->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            dmaInit->Init.Mode = DMA_NORMAL;
            dmaInit->Init.Priority = DMA_PRIORITY_LOW;

            if (HAL_DMA_Init(dmaInit) == HAL_OK) {
                __HAL_LINKDMA(hi2c, hdmatx, *dmaInit);

                HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 7, 0);
                HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
            }

            dmaInit = &dma5Handle;
            dmaInit->Instance = DMA1_Channel3;
            dmaInit->Init.Request = DMA_REQUEST_I2C1_RX;
            dmaInit->Init.Direction = DMA_PERIPH_TO_MEMORY;
            dmaInit->Init.PeriphInc = DMA_PINC_DISABLE;
            dmaInit->Init.MemInc = DMA_MINC_ENABLE;
            dmaInit->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            dmaInit->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            dmaInit->Init.Mode = DMA_NORMAL;
            dmaInit->Init.Priority = DMA_PRIORITY_HIGH;
            if (HAL_DMA_Init(dmaInit) == HAL_OK) {
                __HAL_LINKDMA(hi2c, hdmarx, *dmaInit);

                HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 6, 0);
                HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
            }

            HAL_NVIC_SetPriority(I2C1_EV_IRQn, 8, 0);
            HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_SetPriority(I2C1_ER_IRQn, 9, 0);
            HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
        }
    }
}

/**
 * @brief DeInitialize the I2C interfaces
 * @param hi2c is the pointer to the data structure of the I2C handle (HAL).
 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        __HAL_RCC_I2C1_FORCE_RESET();
        __HAL_RCC_I2C1_RELEASE_RESET();
        __HAL_RCC_I2C1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);

        HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);
        HAL_NVIC_DisableIRQ(DMA1_Channel3_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
    }
}

/**
 * @brief Initialize the CRC module, turn ON a clock source
 * @param hcrc is the pointer to the data structure of the CRC handle (HAL).
 */
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc) {
    if (hcrc->Instance == CRC) {
        __HAL_RCC_CRC_CLK_ENABLE();
    }
}

/**
 * @brief DeInitialize the CRC module
 * @param hcrc is the pointer to the data structure of the CRC handle (HAL).
 */
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc) {
    if (hcrc->Instance == CRC) {
        __HAL_RCC_CRC_FORCE_RESET();
        __HAL_RCC_CRC_RELEASE_RESET();
        __HAL_RCC_CRC_CLK_DISABLE();
    }
}

/**
  * @brief  Initialize the PPP MSP.
  * @param  None
  * @retval None
  */
void HAL_PPP_MspInit(void) {
}

/**
  * @brief  DeInitialize the PPP MSP.
  * @param  None
  * @retval None
  */
void HAL_PPP_MspDeInit(void) {
}
