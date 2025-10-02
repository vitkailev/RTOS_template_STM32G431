#include <string.h>
#include <limits.h>

#include "I2CBusJob.h"

static StaticSemaphore_t mutexCB;
static StaticEventGroup_t eventGroupCB;
static StaticQueue_t queueCB;
static uint8_t queueStorage[I2CBUS_QUEUE_SIZE * sizeof(I2CPackageDef)];
static StaticTask_t taskCB;
static StackType_t taskStack[configMINIMAL_STACK_SIZE];

/**
 * @brief I2C interface (bus) task
 * @param arg is the function argument to which the scheduler will send the specified parameter
 * (while creating the task - I2CBusDef data structure)
 */
static void I2CBusJob(void *arg) {
    I2CBusDef *bus = (I2CBusDef *) arg;

    const TickType_t delay = pdMS_TO_TICKS(I2CBUS_DELAY_MS);
    BaseType_t result = pdFALSE;
    uint32_t notificationValue = 0;
    I2CPackageDef pkg = {0};

    while (1) {
        result = xQueueReceive(bus->queue, &pkg, portMAX_DELAY);
        if (result == pdPASS) {
            if (pkg.data)
                bus->i2c->sendData(bus->i2c, pkg.address, pkg.data, pkg.size, pkg.isNeedStop);
            else
                bus->i2c->readData(bus->i2c, pkg.address, bus->buffer, pkg.size);

            result = xTaskNotifyWait(0, ULONG_MAX, &notificationValue, delay);
            if (result == pdTRUE) {
                if (notificationValue & I2CBUS_NOTIF_TX_FLAG) {
                    bus->isWriting = false;
                }
                if (notificationValue & I2CBUS_NOTIF_RX_FLAG) {
                    bus->isReading = false;
                }
                if (notificationValue & I2CBUS_NOTIF_ERR_FLAG) {
                    bus->i2c->saveError(bus->i2c);
                }
                if (notificationValue & I2CBUS_NOTIF_ABORT_FLAG) {
                    bus->i2c->abort = true;
                }

                xEventGroupSetBits(bus->eventGroup, 1);
            } else {
                bus->errors++;
            }
        }
    }
}

/**
 * @brief Create the I2C interface task and all required structure
 * @param bus is the I2CBusDef data structure
 * @param i2c is the base I2C data structure
 * @param priorityLevel is the priority of the I2C interface task
 * @return pointer to the I2C interface task handle
 */
TaskHandle_t I2CJobInit(I2CBusDef *bus, I2CDef *i2c, uint8_t priorityLevel) {
    bus->i2c = i2c;
    bus->i2c->init(bus->i2c);

    bus->isWriting = bus->isReading = false;
    bus->errors = 0;
    memset((void *) bus->buffer, 0, I2CBUS_BUFFER_SIZE);

    bus->mutex = xSemaphoreCreateMutexStatic(&mutexCB);
    bus->eventGroup = xEventGroupCreateStatic(&eventGroupCB);
    bus->queue = xQueueCreateStatic(I2CBUS_QUEUE_SIZE, sizeof(I2CPackageDef), queueStorage, &queueCB);
    TaskHandle_t task = xTaskCreateStatic(I2CBusJob, "i2cBus", configMINIMAL_STACK_SIZE, bus, priorityLevel,
                                          taskStack, &taskCB);
    return task;
}

/**
 * @brief Check, that I2C interface is sending data
 * @param bus is the I2CBusDef data structure
 * @return True - is writing, otherwise - False
 */
bool I2C_isWriting(const I2CBusDef *bus) {
    if (bus == NULL)
        return true;

    return bus->isWriting;
}

/**
 * @brief Send data
 * @param bus is the I2CBusDef data structure
 * @param addr is the target device address
 * @param src is the target data
 * @param size is the target data size (bytes)
 * @param isNeedStop flag, True - if you need to generate the Stop signal after the data transaction, otherwise - False
 * @return I2C_Errors value
 */
int32_t I2C_writeData(I2CBusDef *bus, uint16_t addr, const void *src, size_t size, bool isNeedStop) {
    if (bus == NULL || src == NULL || size == 0)
        return I2C_WRONG_DATA;

    bus->isWriting = true;
    BaseType_t result = pdFALSE;
    memcpy((void *) bus->buffer, src, size);
    I2CPackageDef pkg = {isNeedStop, addr, (uint16_t) size, bus->buffer};
    result = xQueueSend(bus->queue, (const void *) &pkg, pdMS_TO_TICKS(I2CBUS_DELAY_MS));

    return (result != errQUEUE_FULL) ? I2C_SUCCESS : I2C_HW_ERROR;
}

/**
 * @brief Check, that I2C interface is reading a new data
 * @param bus is the I2CBusDef data structure
 * @return True - is reading, otherwise - False
 */
bool I2C_isReading(const I2CBusDef *bus) {
    if (bus == NULL)
        return true;

    return bus->isReading;
}

/**
 * @brief Read data
 * @param bus is the I2CBusDef data structure
 * @param addr is the target device address
 * @param size is the size of the required data
 * @return I2C_Errors value
 */
int32_t I2C_readData(I2CBusDef *bus, uint16_t addr, size_t size) {
    if (bus == NULL || size == 0)
        return I2C_WRONG_DATA;

    bus->isReading = true;
    BaseType_t result = pdFALSE;

    I2CPackageDef pkg = {false, addr, (uint16_t) size, NULL};
    result = xQueueSend(bus->queue, (void *) &pkg, pdMS_TO_TICKS(I2CBUS_DELAY_MS));

    return (result != errQUEUE_FULL) ? I2C_SUCCESS : I2C_HW_ERROR;
}

/**
 * @brief Get a pointer to the received data
 * @param bus is the I2CBusDef data structure
 * @return a pointer to I2C interface buffer
 */
const void *I2C_getReceivedData(const I2CBusDef *bus) {
    return (const void *) bus->buffer;
}

/**
 * @brief Check, that I2C interface has any problems
 * @param bus is the I2CBusDef data structure
 * @return True - if the interface has catch any error or Abort state, otherwise - False
 */
bool I2C_isFailed(const I2CBusDef *bus) {
    return bus->i2c->isFailed(bus->i2c);
}
