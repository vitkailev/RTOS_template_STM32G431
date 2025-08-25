#include "stm32g4xx_hal.h"

#include "jobs.h"

/**
 * @brief ADC interrupt callback function
 * @param hadc is the ADC handle structure (HAL)
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (hadc->Instance == ((ADC_HandleTypeDef *) Mcu.adc.handle)->Instance) {
        xTaskNotifyFromISR(Mcu.jobs.handles[SENSORS_JOB], JOB_NOTIFY_ADC_FLAG, eSetBits, &priorityTaskWoken);
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}

/**
 * @brief ADC interrupt error callback function
 * @param hadc is the ADC handle structure (HAL)
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (hadc->Instance == ((ADC_HandleTypeDef *) Mcu.adc.handle)->Instance) {
        xTaskNotifyFromISR(Mcu.jobs.handles[SENSORS_JOB], JOB_NOTIFY_ADC_ERR_FLAG, eSetBits, &priorityTaskWoken);
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}
