#ifndef VARIABLES_H
#define VARIABLES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum Constants {
    ANALOG_IN_1 = 0,
    ANALOG_IN_2,
    ANALOG_TEMP_VREF,
    NUMBER_ADC_CHANNELS,
};

typedef struct {
    bool isTriggered;
    bool state;

    uint16_t delay;
    uint16_t duration;

    uint16_t pin;
    void *handle;
} PortDef;

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
    TimerDef timer;
} AdcDef;

typedef struct {
    void *crc;
    void *wdt;
} HandlesDef;

typedef struct {
    int32_t temp;

    PortDef button;
    PortDef led;
    AdcDef adc;
    TimerDef pwm;
    HandlesDef handles;
} McuDef;

#ifdef __cplusplus
}
#endif

#endif //VARIABLES_H
