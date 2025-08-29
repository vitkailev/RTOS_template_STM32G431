#include "stm32g4xx_hal.h"

#include "uart.h"

/**
 * @brief UART interface initialization
 * @param uart is the base UART data structure
 * @return UART_Error value
 */
static int32_t UART_init(UartDef *uart) {
    if (uart == NULL)
        return UART_WRONG_DATA;

    uart->errType = UART_SUCCESS;
    uart->errors = 0;

    uart->isInit = true;
    return UART_SUCCESS;
}

/**
 * @brief Sending the required data via UART interface
 * @param uart is the base UART data structure
 * @param src is the source buffer
 * @param size is the required data size (bytes)
 * @return UART_Error value
 */
static int32_t UART_sendData(UartDef *uart, const void *src, size_t size) {
    if (uart == NULL || src == NULL || size == 0)
        return UART_WRONG_DATA;

    HAL_StatusTypeDef result = HAL_ERROR;
    result = HAL_UART_Transmit_DMA((UART_HandleTypeDef *) uart->handle, (const uint8_t *) src, (uint16_t) size);
    return (result == HAL_OK) ? UART_SUCCESS : UART_HW_ERROR;
}

/**
 * @brief Reading any data via UART interface
 * @param uart is the base UART data structure
 * @param dst is the destination buffer
 * @param size is the destination buffer size (bytes)
 * @return UART_Error value
 */
static int32_t UART_readData(UartDef *uart, void *dst, size_t size) {
    if (uart == NULL || dst == NULL || size == 0)
        return UART_WRONG_DATA;

    HAL_StatusTypeDef result = HAL_ERROR;
    result = HAL_UARTEx_ReceiveToIdle_DMA((UART_HandleTypeDef *) uart->handle, (uint8_t *) dst, (uint16_t) size);
    return (result == HAL_OK) ? UART_SUCCESS : UART_HW_ERROR;
}

/**
 * @brief Save the last error type of the UART interface and update its counter
 * @param uart is the base UART data structure
 * @return UART_Error value
 */
static int32_t UART_saveError(UartDef *uart) {
    if (uart == NULL)
        return UART_WRONG_DATA;

    uint32_t err = HAL_UART_GetError((UART_HandleTypeDef *) uart->handle);
    if (err != HAL_UART_ERROR_NONE) {
        uart->errType = (int32_t) err;
        uart->errors++;
    }
    return UART_SUCCESS;
}

/**
 * @brief Get the last error type of the UART interface
 * @param uart is the base UART data structure
 * @return error type
 */
static int32_t UART_getErrorType(const UartDef *uart) {
    if (uart == NULL)
        return UART_WRONG_DATA;

    return uart->errType;
}

/**
 * @brief Get the current number of UART interface errors ever detected
 * @param uart is the base UART data structure
 * @return number of errors
 */
static int32_t UART_getNumOfErrors(const UartDef *uart) {
    if (uart == NULL)
        return UART_WRONG_DATA;

    return uart->errors;
}

static UART_HandleTypeDef uart1Handle;

UartDef UART1_intf = {
    &uart1Handle, false, UART_NOT_INIT, 1,
    UART_init, UART_sendData, UART_readData,
    UART_saveError, UART_getErrorType, UART_getNumOfErrors,
};
