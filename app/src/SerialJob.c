#include <string.h>
#include <limits.h>

#include "SerialJob.h"

static StaticSemaphore_t mutexBufferRx;
static StaticStreamBuffer_t streamBufferRx;
static uint8_t streamBufferRx_storage[SERIAL_PORT_STREAM_SIZE];
static StaticSemaphore_t mutexBufferTx;
static StaticStreamBuffer_t streamBufferTx;
static uint8_t streamBufferTx_storage[SERIAL_PORT_STREAM_SIZE];
static StaticTask_t taskTCB;
static StackType_t taskStack[configMINIMAL_STACK_SIZE];

/**
 * @brief Serial Port task
 * @param arg is the function argument to which the scheduler will send the specified parameter
 * (while creating the task - SerialPort data structure)
 */
static void SerialJob(void *arg) {
    SerialPortDef *port = (SerialPortDef *) arg;

    const TickType_t delay = pdMS_TO_TICKS(SERIAL_PORT_DELAY_MS);
    BaseType_t result = pdFALSE;
    uint32_t notificationValue = 0;
    uint32_t numBytes = 0;
    bool isTransactionLocked = true;

    // start to read/wait data via UART interface
    port->uart->readData(port->uart, port->rxBuffer, SERIAL_PORT_BUFFER_SIZE);

    while (1) {
        result = xTaskNotifyWait(0, ULONG_MAX, &notificationValue, delay);
        if (result == pdTRUE) {
            if (notificationValue & SERIAL_NOTIF_TX_FLAG) {
                numBytes = xStreamBufferReceive(port->txStream, port->txBuffer, SERIAL_PORT_BUFFER_SIZE, delay);
                if (numBytes) {
                    port->uart->sendData(port->uart, port->txBuffer, numBytes);
                } else {
                    isTransactionLocked = true;
                }
            }

            if (notificationValue & SERIAL_NOTIF_RX_FLAG) {
                port->uart->readData(port->uart, port->rxBuffer, SERIAL_PORT_BUFFER_SIZE);
            }

            if (notificationValue & SERIAL_NOTIF_ERR_FLAG) {
                port->uart->saveError(port->uart);
            }

            if (notificationValue & SERIAL_NOTIF_ABORT_FLAG) {
            }
        } else {
            if (xSemaphoreTake(port->txMutex, delay / 2) == pdPASS) {
                if (isTransactionLocked && !xStreamBufferIsEmpty(port->txStream)) {
                    isTransactionLocked = false;
                    xTaskNotify(xTaskGetCurrentTaskHandle(), SERIAL_NOTIF_TX_FLAG, eSetBits);
                }
                xSemaphoreGive(port->txMutex);
            }
        }
    }
}

/**
 * @brief Create the Serial Port task and all required structure
 * @param port is the SerialPort data structure
 * @param uart is the base UART data structure
 * @param priorityLevel is the priority of the Serial Port task
 * @return pointer to the Serial Port task handle
 */
TaskHandle_t SerialJobInit(SerialPortDef *port, UartDef *uart, uint8_t priorityLevel) {
    port->uart = uart;
    port->uart->init(port->uart);

    port->errors = 0;
    memset(port->rxBuffer, 0, SERIAL_PORT_BUFFER_SIZE);
    memset(port->txBuffer, 0, SERIAL_PORT_BUFFER_SIZE);

    port->rxMutex = xSemaphoreCreateMutexStatic(&mutexBufferRx);
    port->rxStream = xStreamBufferCreateStatic(SERIAL_PORT_STREAM_SIZE, SERIAL_PORT_TRIGGER_LEVEL,
                                               streamBufferRx_storage, &streamBufferRx);
    port->txMutex = xSemaphoreCreateMutexStatic(&mutexBufferTx);
    port->txStream = xStreamBufferCreateStatic(SERIAL_PORT_STREAM_SIZE, SERIAL_PORT_TRIGGER_LEVEL,
                                               streamBufferTx_storage, &streamBufferTx);

    TaskHandle_t task = xTaskCreateStatic(SerialJob, "serialPort", configMINIMAL_STACK_SIZE, port, priorityLevel,
                                          taskStack, &taskTCB);
    return task;
}

/**
 * @brief Send the required data to the Serial Port output buffer/stream
 * @param port is the SerialPort data structure
 * @param src is the source buffer
 * @param size is the required data size (bytes)
 * @return the number of bytes that were sent
 */
int32_t SerialWriteData(SerialPortDef *port, const void *src, size_t size) {
    if (port == NULL || src == NULL || size == 0)
        return -1;

    const uint8_t *data = (const uint8_t *) src;
    const TickType_t delay = pdMS_TO_TICKS(SERIAL_PORT_DELAY_MS);
    const TickType_t endTime = xTaskGetTickCount() + delay;
    uint32_t numBytes = 0;

    if (xSemaphoreTake(port->txMutex, delay) == pdPASS) {
        TickType_t remainTime = endTime - xTaskGetTickCount();
        numBytes = xStreamBufferSend(port->txStream, data, size, remainTime);

        if (numBytes != size) {
            remainTime = endTime - xTaskGetTickCount();
            numBytes += xStreamBufferSend(port->txStream, data + numBytes, size - numBytes, remainTime);
        }

        xSemaphoreGive(port->txMutex);
    } else {
        port->errors++;
    }

    return (int32_t) numBytes;
}

/**
 * @brief Read received data from the Serial Port input buffer/stream
 * @param port is the SerialPort data structure
 * @param dst is the destination buffer
 * @param size is the destination buffer size (bytes)
 * @return the number of bytes that were read
 */
int32_t SerialReadData(SerialPortDef *port, void *dst, size_t size) {
    if (port == NULL || dst == NULL || size == 0)
        return -1;

    uint8_t *data = (uint8_t *) dst;
    const TickType_t delay = pdMS_TO_TICKS(SERIAL_PORT_DELAY_MS);
    uint32_t numBytes = 0;

    if (xSemaphoreTake(port->rxMutex, delay) == pdPASS) {
        numBytes = xStreamBufferReceive(port->rxStream, data, size, delay);
        xSemaphoreGive(port->rxMutex);
    }

    return (int32_t) numBytes;
}
