#include "stm32g4xx_hal.h"

#include "settings.h"

static CRC_HandleTypeDef crcHandle;
static IWDG_HandleTypeDef wdtHandle;

/**
 * @brief Setting system clocks
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingSystemClock(void) {
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef oscInit = {0};
    oscInit.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    oscInit.LSEState = RCC_LSE_OFF;
    oscInit.HSEState = RCC_HSE_OFF;
    oscInit.HSI48State = RCC_HSI48_OFF;
    oscInit.LSIState = RCC_LSI_ON;
    oscInit.HSIState = RCC_HSI_ON;
    oscInit.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    // Datasheet, DS12589 Rev. 6, Electrical characteristics, PLL characteristics, page 106
    // Voltage scaling Range 1: 150MHz
    oscInit.PLL.PLLState = RCC_PLL_ON;
    oscInit.PLL.PLLSource = RCC_PLLSOURCE_HSI; // 16MHz
    oscInit.PLL.PLLM = RCC_PLLM_DIV1;
    oscInit.PLL.PLLN = 18; // PLL VCO: 288MHz

    // Datasheet, DS12589 Rev. 6, Electrical characteristics, Analog-to-digital converter characteristics, page 118
    // Range 1, Vdda >= 2.7V, max 52MHz
    oscInit.PLL.PLLP = RCC_PLLP_DIV6; // 48MHz

    oscInit.PLL.PLLQ = RCC_PLLQ_DIV6;
    oscInit.PLL.PLLR = RCC_PLLR_DIV2; // PLLCLK: 144NHz

    if (HAL_RCC_OscConfig(&oscInit) != HAL_OK)
        return SETTING_ERROR;

    RCC_ClkInitTypeDef clkInit = {0};
    clkInit.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

    clkInit.AHBCLKDivider = RCC_SYSCLK_DIV1; // HCLK = 144MHz
    clkInit.APB1CLKDivider = RCC_HCLK_DIV4; // 36MHz
    clkInit.APB2CLKDivider = RCC_HCLK_DIV4; // 36MHz

    if (HAL_RCC_ClockConfig(&clkInit, FLASH_LATENCY_4) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting GPIO pins (input and output)
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingGPIO(void) {
    GPIO_InitTypeDef gpioInit = {0};

    // Button
    __HAL_RCC_GPIOC_CLK_ENABLE();
    gpioInit.Pin = GPIO_PIN_13;
    gpioInit.Mode = GPIO_MODE_INPUT;
    gpioInit.Pull = GPIO_PULLDOWN;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpioInit);

    // LED
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpioInit.Pin = GPIO_PIN_5;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull = GPIO_PULLDOWN;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpioInit);

    return SETTING_SUCCESS;
}

/**
 * @brief Setting Cyclic-Redundancy-Check (CRC) module
 * @param mcu is the base MCU data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingCRC(McuDef *mcu) {
    mcu->handles.crc = &crcHandle;
    CRC_HandleTypeDef *crcInit = (CRC_HandleTypeDef *) mcu->handles.crc;
    crcInit->Instance = CRC;
    crcInit->InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES; // uint8_t
    crcInit->Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
    crcInit->Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    crcInit->Init.CRCLength = CRC_POLYLENGTH_32B;
    crcInit->Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
    crcInit->Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;

    if (HAL_CRC_Init(&crcHandle) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting watchdog timer (WDT) module
 * @param mcu is the base MCU data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingWDT(McuDef *mcu) {
    mcu->handles.wdt = (void *) &wdtHandle;
    IWDG_HandleTypeDef *wdtInit = (IWDG_HandleTypeDef *) mcu->handles.wdt;
    wdtInit->Instance = IWDG;
    wdtInit->Init.Prescaler = IWDG_PRESCALER_8;
    wdtInit->Init.Reload = 0x0FFF;
    wdtInit->Init.Window = 0x0FFF; // window mode is turn OFF

    if (HAL_IWDG_Init(wdtInit) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

int turnOnInterrupts(McuDef *mcu) {
    return SETTING_SUCCESS;
}

int initialization(McuDef *mcu) {
    if (settingSystemClock() != SETTING_SUCCESS) {
    } else if (settingGPIO() != SETTING_SUCCESS) {
    } else if (settingCRC(mcu) != SETTING_SUCCESS) {
    } else if (settingWDT(mcu) != SETTING_SUCCESS) {
    } else {
        return turnOnInterrupts(mcu);
    }

    return SETTING_ERROR;
}
