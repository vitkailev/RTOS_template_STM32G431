#include "stm32g4xx_hal.h"

#include "utilities.h"

/**
 * @brief Set the state/level of the output pin
 * @param port is the base GPIO port data structure
 * @param state is a pin state (True - HIGH or False - LOW)
 */
void changePinState(PortDef *port, bool state) {
    port->state = state;
    HAL_GPIO_WritePin((GPIO_TypeDef *) port->handle, port->pin, state);
}

/**
 * @brief Check if the input pin has changed state (detect this event)
 * @param port is the base GPIO port data structure
 * @return True - a pin changed state from LOW to HIGH, otherwise - False
 */
bool isPinTriggered(const PortDef *port) {
    return port->isTriggered;
}

/**
 * @brief Get/read the current state of the input pin
 * @param port is the base GPIO port data structure
 * @return True - a pin is HIGH, otherwise - LOW
 */
bool getPinState(const PortDef *port) {
    return port->state;
}

/**
 * @brief Update the GPIO pin data structure state
 * @param port is the base GPIO port data structure
 */
void checkPinState(PortDef *port) {
    bool isSet = HAL_GPIO_ReadPin((GPIO_TypeDef *) port->handle, port->pin);
    if (isSet) {
        port->delay++;
    } else {
        port->delay = 0;

        // if you need to control the moment of release
        // port->isTriggered = port->state;

        port->state = false;
    }

    if (port->delay == port->duration) {
        port->isTriggered = !port->state;
        port->state = true;
    }
}

/**
 * @brief Set the duty value of the PWM signal
 * @param tim is the base timer data structure
 * @param channel is the timer output channel (TIM_CHANNEL_x (x=1..6) or TIM_CHANNEL_ALL)
 * @param value is the duty value (from 0 to 100)
 */
void setPWMDutyCycle(TimerDef *tim, uint16_t channel, uint8_t value) {
    if (value >= 100)
        value = 99;
    else if (value == 0)
        value = 1;
    __HAL_TIM_SET_COMPARE((TIM_HandleTypeDef *) tim->handle, channel, value);
}

/**
 * @brief Get the MCU Unique ID
 * @return pointer to a unique id value
 */
const void *getUniqueID(void) {
    // RM0440 Reference manual, 48.1 Unique device ID, page 2121
    return (const void *) 0x1FFF7590;
}

/**
 * @brief Calculate CRC-32
 * @param handle is the pointer to CRC module handle (HAL)
 * @param data is target data
 * @param size is target data size (bytes)
 * @return if successfully - CRC value, otherwise - 0
 */
uint32_t getCRC(void *handle, const void *data, uint16_t size) {
    // https://crccalc.com/
    // CRC-32

    if (handle == NULL || data == NULL || size == 0)
        return 0;

    uint32_t result = HAL_CRC_Calculate((CRC_HandleTypeDef *) handle, (uint32_t *) data, size);
    HAL_CRC_StateTypeDef state = HAL_CRC_GetState((CRC_HandleTypeDef *) handle);
    return (state == HAL_CRC_STATE_READY) ? (result ^ 0xFFFFFFFFU) : 0;
}
