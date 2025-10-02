#ifndef I2CBUSJOB_H
#define I2CBUSJOB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

#include "i2c.h"

enum I2CBus_Constants {
    I2CBUS_BUFFER_SIZE = 32,
    I2CBUS_QUEUE_SIZE = 8,
    I2CBUS_DELAY_MS = 10,

    I2CBUS_NOTIF_TX_FLAG = 1 << 0,
    I2CBUS_NOTIF_RX_FLAG = 1 << 1,
    I2CBUS_NOTIF_ERR_FLAG = 1 << 2,
    I2CBUS_NOTIF_ABORT_FLAG = 1 << 3,
};

typedef struct {
    bool isNeedStop;
    uint16_t address;
    uint16_t size;
    uint8_t *data;
} I2CPackageDef;

typedef struct {
    I2CDef *i2c;

    bool isWriting;
    bool isReading;
    uint32_t errors;

    // a temporary buffer to send/read data via I2C  - DMA
    uint8_t buffer[I2CBUS_BUFFER_SIZE];

    SemaphoreHandle_t mutex;
    EventGroupHandle_t eventGroup;
    QueueHandle_t queue;
} I2CBusDef;

extern I2CDef I2C1_intf;

TaskHandle_t I2CJobInit(I2CBusDef *bus, I2CDef *i2c, uint8_t priorityLevel);

bool I2C_isWriting(const I2CBusDef *bus);

int32_t I2C_writeData(I2CBusDef *bus, uint16_t addr, const void *src, size_t size, bool isNeedStop);

bool I2C_isReading(const I2CBusDef *bus);

int32_t I2C_readData(I2CBusDef *bus, uint16_t addr, size_t size);

const void *I2C_getReceivedData(const I2CBusDef *bus);

bool I2C_isFailed(const I2CBusDef *bus);

#ifdef __cplusplus
}
#endif

#endif // I2CBUSJOB_H
