#include "stm32g4xx_hal.h"

#include "jobs.h"

/**
 * @brief ADC interrupt callback function
 * @param hadc is the ADC handle structure (HAL)
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (hadc->Instance == ((ADC_HandleTypeDef *) Application.hardware.adc.handle)->Instance) {
        xTaskNotifyFromISR(Application.handles[SENSORS_JOB], JOB_NOTIF_SENSOR_FLAG, eSetBits, &priorityTaskWoken);
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}

/**
 * @brief ADC interrupt error callback function
 * @param hadc is the ADC handle structure (HAL)
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (hadc->Instance == ((ADC_HandleTypeDef *) Application.hardware.adc.handle)->Instance) {
        xTaskNotifyFromISR(Application.handles[SENSORS_JOB], JOB_NOTIF_SENSOR_ERR_FLAG, eSetBits, &priorityTaskWoken);
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}

/**
 * @brief The transmission callback function
 * @param huart is the UART handle structure (HAL)
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (huart->Instance == ((UART_HandleTypeDef *) Serial.uart->handle)->Instance) {
        xTaskNotifyFromISR(Application.handles[SERIAL_PORT_JOB], SERIAL_NOTIF_TX_FLAG, eSetBits, &priorityTaskWoken);
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}

/**
 * @brief The reception callback function (Rx event notification called after use of advanced reception service)
 * @param huart is the UART handle structure (HAL)
 * @param Size is the number of data available in application reception buffer
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (huart->Instance == ((UART_HandleTypeDef *) Serial.uart->handle)->Instance) {
        switch (huart->RxEventType) {
            case HAL_UART_RXEVENT_TC:
                break;
            case HAL_UART_RXEVENT_HT:
                break;
            case HAL_UART_RXEVENT_IDLE:
                xStreamBufferSendFromISR(Serial.rxStream, Serial.rxBuffer, Size, &priorityTaskWoken);
                xTaskNotifyFromISR(Application.handles[SERIAL_PORT_JOB], SERIAL_NOTIF_RX_FLAG, eSetBits,
                                   &priorityTaskWoken);
                break;
            default:
                break;
        }
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}

/**
 * @brief The UART error callback function
 * @param huart is the UART handle structure (HAL)
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (huart->Instance == ((UART_HandleTypeDef *) Serial.uart->handle)->Instance) {
        xTaskNotifyFromISR(Application.handles[SERIAL_PORT_JOB], SERIAL_NOTIF_ERR_FLAG, eSetBits, &priorityTaskWoken);
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}

/**
 * @brief The UART abort complete callback function
 * @param huart is the UART handle structure (HAL)
 */
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart) {
    BaseType_t priorityTaskWoken = pdFALSE;

    if (huart->Instance == ((UART_HandleTypeDef *) Serial.uart->handle)->Instance) {
        xTaskNotifyFromISR(Application.handles[SERIAL_PORT_JOB], SERIAL_NOTIF_ABORT_FLAG, eSetBits, &priorityTaskWoken);
    }

    portYIELD_FROM_ISR(priorityTaskWoken);
}
