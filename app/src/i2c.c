#include "stm32g4xx_hal.h"

#include "i2c.h"

/**
 * @brief I2C interface initialization
 * @param i2c is the base I2C data structure
 * @return I2C_Error value
 */
static int32_t I2C_init(I2CDef *i2c) {
    if (i2c == NULL)
        return I2C_WRONG_DATA;

    i2c->errType = HAL_I2C_ERROR_NONE;
    i2c->errors = 0;

    i2c->isInit = true;
    return I2C_SUCCESS;
}

/**
 * @brief Sending the required data via I2C interface
 * @param i2c is the base I2C data structure
 * @param addr is the device address on the I2C bus
 * @param src is the source buffer
 * @param size is the required data size (bytes)
 * @param isNeedStop
 * @return I2C_Error value
 */
static int32_t I2C_sendData(I2CDef *i2c, uint16_t addr, const void *src, size_t size, bool isNeedStop) {
    if (i2c == NULL || src == NULL || size == 0)
        return I2C_WRONG_DATA;

    i2c->errType = HAL_I2C_ERROR_NONE;
    i2c->abort = false;

    HAL_StatusTypeDef result = HAL_ERROR;
    result = HAL_I2C_Master_Seq_Transmit_DMA((I2C_HandleTypeDef *) i2c->handle, addr << 1,
                                             (uint8_t *) src, (uint16_t) size,
                                             (isNeedStop) ? I2C_FIRST_AND_LAST_FRAME : I2C_FIRST_FRAME);

    return (result == HAL_OK) ? I2C_SUCCESS : I2C_HW_ERROR;
}

/**
 * @brief Reading any data via I2C interface
 * @param i2c is the base I2C data structure
 * @param addr is the device address on the I2C bus
 * @param dst is the destination buffer
 * @param size is the destination buffer size (bytes)
 * @return I2C_Error value
 */
static int32_t I2C_readData(I2CDef *i2c, uint16_t addr, void *dst, size_t size) {
    if (i2c == NULL || dst == 0 || size == 0)
        return I2C_WRONG_DATA;

    i2c->errType = HAL_I2C_ERROR_NONE;
    i2c->abort = false;

    HAL_StatusTypeDef result = HAL_ERROR;
    result = HAL_I2C_Master_Seq_Receive_DMA((I2C_HandleTypeDef *) i2c->handle, addr << 1,
                                            (uint8_t *) dst, (uint16_t) size,
                                            I2C_LAST_FRAME);

    return (result == HAL_OK) ? I2C_SUCCESS : I2C_HW_ERROR;
}

/**
 * @brief Save the last error type of the I2C interface and update its counter
 * @param i2c is the base I2C data structure
 * @return I2C_Error value
 */
static int32_t I2C_saveError(I2CDef *i2c) {
    if (i2c == NULL)
        return I2C_WRONG_DATA;

    uint32_t err = HAL_I2C_GetError((I2C_HandleTypeDef *) i2c->handle);
    if (err != HAL_I2C_ERROR_NONE) {
        i2c->errType = (int32_t) err;
        i2c->errors++;
    }
    return I2C_SUCCESS;
}

/**
 * @brief Get the last error type of the I2C interface
 * @param i2c is the base I2C data structure
 * @return error type
 */
static int32_t I2C_getErrorType(const I2CDef *i2c) {
    if (i2c == NULL)
        return I2C_WRONG_DATA;

    return i2c->errType;
}

/**
 * @brief Get the current number of I2C interface errors ever detected
 * @param i2c is the base I2C data structure
 * @return number of errors
 */
static int32_t I2C_getNumOfErrors(const I2CDef *i2c) {
    if (i2c == NULL)
        return I2C_WRONG_DATA;

    return i2c->errors;
}

/**
 * @brief Check, that I2C interface has any problems
 * @param i2c is the base I2C data structure
 * @return True - an error occurred or the transaction was aborted, otherwise - False
 */
static int32_t I2C_isFailed(const I2CDef *i2c) {
    if (i2c == NULL)
        return I2C_WRONG_DATA;

    return (i2c->errType || i2c->abort);
}

static I2C_HandleTypeDef i2c1Handle;

I2CDef I2C1_intf = {
    &i2c1Handle, false,false, I2C_NOT_INIT, 1,
    I2C_init, I2C_sendData, I2C_readData,
    I2C_saveError, I2C_getErrorType, I2C_getNumOfErrors, I2C_isFailed
};
