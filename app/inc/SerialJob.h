#ifndef SERIALJOB_H
#define SERIALJOB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stream_buffer.h"

#include "uart.h"

enum SerialPort_Constants {
    SERIAL_PORT_BUFFER_SIZE = 128,

    SERIAL_PORT_STREAM_SIZE = 1024,
    SERIAL_PORT_TRIGGER_LEVEL = SERIAL_PORT_BUFFER_SIZE / 2, // stream buffer

    SERIAL_PORT_DELAY_MS = 100,

    SERIAL_NOTIF_TX_FLAG = 1 << 0,
    SERIAL_NOTIF_RX_FLAG = 1 << 1,
    SERIAL_NOTIF_ERR_FLAG = 1 << 2,
    SERIAL_NOTIF_ABORT_FLAG = 1 << 3,
};

typedef struct {
    UartDef *uart;

    uint32_t errors;

    // a temporary buffer to send/read data via UART - DMA
    uint8_t rxBuffer[SERIAL_PORT_BUFFER_SIZE];
    uint8_t txBuffer[SERIAL_PORT_BUFFER_SIZE];

    SemaphoreHandle_t rxMutex;
    StreamBufferHandle_t rxStream;
    SemaphoreHandle_t txMutex;
    StreamBufferHandle_t txStream;
} SerialPortDef;

extern UartDef UART1_intf;

TaskHandle_t SerialJobInit(SerialPortDef *port, UartDef *uart, uint8_t priorityLevel);

int32_t SerialWriteData(SerialPortDef *port, const void *src, size_t size);

int32_t SerialReadData(SerialPortDef *port, void *dst, size_t size);

#ifdef __cplusplus
}
#endif

#endif //SERIALJOB_H
