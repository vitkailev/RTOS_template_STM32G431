#include <limits.h>

#include "stm32g4xx_hal.h"

#include "jobs.h"
#include "utilities.h"

static StaticTask_t task1CB;
static StackType_t task1Stack[configMINIMAL_STACK_SIZE];
static StaticTask_t task2CB;
static StackType_t task2Stack[configMINIMAL_STACK_SIZE];
static StaticTask_t task3CB;
static StackType_t task3Stack[configMINIMAL_STACK_SIZE];
static StaticTask_t task4CB;
static StackType_t task4Stack[configMINIMAL_STACK_SIZE];

static StaticTask_t idleCB;
static StackType_t idleStack[configMINIMAL_STACK_SIZE];
static StaticTask_t timerCB;
static StackType_t timerStack[configTIMER_TASK_STACK_DEPTH];

/**
 * @brief Routine task
 * @param arg is the function argument to which the scheduler will send the specified parameter
 */
static void routineJob(void *arg) {
    McuDef *mcu = (McuDef *) arg;

    const TickType_t delay = pdMS_TO_TICKS(ROUTINE_DELAY_MS);

    const char *text = "[Thread 0] Push the button\n\r";

    while (1) {
        vTaskDelay(delay);

        HAL_IWDG_Refresh((IWDG_HandleTypeDef *) mcu->handles.wdt);

        checkPinState(&mcu->button);
        if (isPinTriggered(&mcu->button)) {
            mcu->button.isTriggered = false;
            SerialWriteData(&Serial, text, 28);
        }
    }
}

/**
 * @brief Sensors task
 * @param arg is the function argument to which the scheduler will send the specified parameter
 */
static void sensorsJob(void *arg) {
    McuDef *mcu = (McuDef *) arg;

    const TickType_t delay = pdMS_TO_TICKS(SENSORS_DELAY_MS);
    const TickType_t notifDelay = pdMS_TO_TICKS(SENSORS_NOTIF_DELAY_MS);
    BaseType_t result = pdFALSE;
    uint32_t notificationValue = 0;

    HAL_TIM_Base_Start((TIM_HandleTypeDef *) mcu->adc.timer.handle);
    HAL_ADC_Start_DMA((ADC_HandleTypeDef *) mcu->adc.handle, mcu->adc.rawValues, NUMBER_ADC_CHANNELS);
    HAL_TIM_PWM_Start((TIM_HandleTypeDef *) mcu->pwm.handle, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start((TIM_HandleTypeDef *) mcu->pwm.handle, TIM_CHANNEL_1);

    while (1) {
        vTaskDelay(delay);

        result = xTaskNotifyWait(0x00, ULONG_MAX, &notificationValue, notifDelay);
        if (result == pdTRUE) {
            if (notificationValue & JOB_NOTIF_SENSOR_FLAG) {
                // calculate the analog input pins values
                uint32_t value = 0;
                for (size_t i = 0; i < (NUMBER_ADC_CHANNELS - 1); ++i) {
                    value = mcu->adc.rawValues[i];
                    value = (value * VDD_VALUE) >> 12;
                    mcu->adc.values[i] = (uint16_t) value;
                }

                // calculate the temperature of the build-in temperature sensor (in Celsius)
                mcu->temp = __HAL_ADC_CALC_TEMPERATURE(VDD_VALUE, mcu->adc.rawValues[NUMBER_ADC_CHANNELS - 1],
                                                       ADC_RESOLUTION_12B);

                // update the PWM duty cycle value
                value = mcu->adc.rawValues[ANALOG_IN_2];
                value = (value * 100) >> 12;
                setPWMDutyCycle(&mcu->pwm, TIM_CHANNEL_1, (uint8_t) value);
            }
            if (notificationValue & JOB_NOTIF_SENSOR_ERR_FLAG) {
                mcu->adc.errType = HAL_ADC_GetError((ADC_HandleTypeDef *) mcu->adc.handle);
                mcu->adc.errors++;
            }
        }
    }
}

/**
 * @brief Communication task
 * @param arg is the function argument to which the scheduler will send the specified parameter
 */
static void communicationJob(void *arg) {
    McuDef *mcu = (McuDef *) arg;

    const TickType_t delay = pdMS_TO_TICKS(COMMUNICATION_DELAY_MS);

    const char *text = "[Thread 2] Input data:";
    int32_t numBytes = 0;
    uint8_t buff[64] = {0};

    while (1) {
        vTaskDelay(delay);

        numBytes = SerialReadData(&Serial, buff, 64);
        if (numBytes) {
            SerialWriteData(&Serial, text, 22);
            SerialWriteData(&Serial, buff, numBytes);
        }
    }
}

/**
 * @brief Service task for updating the status of sensors
 * @param arg is the function argument to which the scheduler will send the specified parameter
 */
static void serviceJob(void *arg) {
    SensorsDef *sensors = (SensorsDef *) arg;

    const TickType_t delay = pdMS_TO_TICKS(SERVICE_DELAY_MS);
    EventBits_t event = 0;

    while (1) {
        event = xEventGroupWaitBits(sensors->interface.eventGroup, 1,pdTRUE,pdFALSE, delay);

        if (event & 1) {
        }
    }
}

/**
 * @brief Create the prepared task
 * @param jobs is the JobsDef data structure
 * @return 0 - success
 */
int createJobs(JobsDef *jobs) {
    jobs->handles[ROUTINE_JOB] = xTaskCreateStatic(routineJob, "routine",
                                                   configMINIMAL_STACK_SIZE, (void *) &jobs->hardware,
                                                   tskIDLE_PRIORITY + 2, task1Stack, &task1CB);
    jobs->handles[SENSORS_JOB] = xTaskCreateStatic(sensorsJob, "sensors",
                                                   configMINIMAL_STACK_SIZE, (void *) &jobs->hardware,
                                                   tskIDLE_PRIORITY + 4, task2Stack, &task2CB);
    jobs->handles[COMMUNICATION_JOB] = xTaskCreateStatic(communicationJob, "communication",
                                                         configMINIMAL_STACK_SIZE, (void *) &jobs->hardware,
                                                         tskIDLE_PRIORITY + 1, task3Stack, &task3CB);
    jobs->handles[SERIAL_PORT_JOB] = SerialJobInit(&Serial, &UART1_intf, tskIDLE_PRIORITY + 3);
    jobs->handles[I2CBUS_JOB] = I2CJobInit(&Sensors.interface, &I2C1_intf, tskIDLE_PRIORITY + 3);
    jobs->handles[SERVICE_JOB] = xTaskCreateStatic(serviceJob, "service",
                                                   configMINIMAL_STACK_SIZE, (void *) &Sensors,
                                                   tskIDLE_PRIORITY + 4, task4Stack, &task4CB);

    changePinState(&jobs->hardware.led, GPIO_PIN_SET);
    return 0;
}

/**
 * @brief The function is used to provide the memory for the RTOS Idle task
 * @param ppxIdleTaskTCBBuffer
 * @param ppxIdleTaskStackBuffer
 * @param pulIdleTaskStackSize
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &idleCB;
    *ppxIdleTaskStackBuffer = idleStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/**
 * @brief The function is used to provide the memory for the RTOS Daemon/Timer Service task
 * (if configUSE_TIMERS is set to 1)
 * @param ppxTimerTaskTCBBuffer
 * @param ppxTimerTaskStackBuffer
 * @param pulTimerTaskStackSize
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    *ppxTimerTaskTCBBuffer = &timerCB;
    *ppxTimerTaskStackBuffer = timerStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/**
 * @brief The function is used to provide a place to implement timer functionality
 * (if configUSE_TICK_HOOK is set to 1)
 */
void vApplicationTickHook(void) {
    // Call it every FreeRTOS tick (1 kHz)
    HAL_IncTick();
}

/**
 * @brief The function will be called if FreeRTOS ever detect the stack overflow situation
 * (if configCHECK_FOR_STACK_OVERFLOW is set to 1/2)
 * @param pxTask is the handle of the offending task
 * @param pcTaskName is the name of the offending task
 */
void vApplicationStackOverflowHook(TaskHandle_t *pxTask, char *pcTaskName) {
    __disable_irq();

    while (1) {
    }
}

/**
 * @brief The function will be called if pvPortMalloc() ever returns NULL
 * (if configUSE_MALLOC_FAILED_HOOK is set to 1)
 */
void vApplicationMallocFailedHook(void) {
    __disable_irq();

    while (1) {
    }
}
