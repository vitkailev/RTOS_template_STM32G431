#ifndef UTILITIES_H
#define UTILITIES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "variables.h"

void changePinState(PortDef *port, bool state);

bool isPinTriggered(const PortDef *port);

bool getPinState(const PortDef *port);

void checkPinState(PortDef *port);

void setPWMDutyCycle(TimerDef *tim, uint16_t channel, uint8_t value);

const void *getUniqueID(void);

uint32_t getCRC(void *handle, const void *data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif //UTILITIES_H
