#ifndef VARIABLES_H
#define VARIABLES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

enum Constants {
    TIME_DELAY_MS = 50,

    NUMBER_ADC_CHANNELS = 3,

    ROUTINE_JOB = 0,
    SENSORS_JOB,
    COMMUNICATION_JOB,
    NUMBER_JOBS,
};

typedef struct {
    uint32_t errors; // something expired, if xTicksToWait != portMAX_DELAY

    TaskHandle_t handles[NUMBER_JOBS];
} JobsDef;

typedef struct {
    uint32_t errors;

    uint16_t basePrescaler; // initialization value - "const"
    uint16_t currentPrescaler; // mutable value - via setTimerPrescaler function
    uint32_t freq; // Hz

    void *handle;
} TimerDef;

typedef struct {
    uint32_t errType;
    uint32_t errors;

    uint32_t rawValues[NUMBER_ADC_CHANNELS]; // relative values
    uint16_t values[NUMBER_ADC_CHANNELS]; // mV

    void *handle;
    void *dmaHandle;
    TimerDef timer;
} AdcDef;

typedef struct {
    void *crc;
    void *wdt;
} HandlesDef;

typedef struct {
    uint32_t runtime;
    int32_t temp;

    AdcDef adc;
    HandlesDef handles;

    JobsDef jobs;
} McuDef;

extern McuDef Mcu;

#ifdef __cplusplus
}
#endif

#endif //VARIABLES_H
