#include <limits.h>

#include "stm32g4xx_hal.h"

#include "jobs.h"

static StaticTask_t task1TCB;
static StackType_t task1Stack[configMINIMAL_STACK_SIZE];
static StaticTask_t task2TCB;
static StackType_t task2Stack[configMINIMAL_STACK_SIZE];
static StaticTask_t task3TCB;
static StackType_t task3Stack[configMINIMAL_STACK_SIZE];

static StaticTask_t idleTCB;
static StackType_t idleStack[configMINIMAL_STACK_SIZE];
static StaticTask_t timerTCB;
static StackType_t timerStack[configTIMER_TASK_STACK_DEPTH];

static void routineJob(void *arg) {
    McuDef *mcu = (McuDef *) arg;

    TickType_t period = pdMS_TO_TICKS(10); // ms

    while (1) {
        vTaskDelay(period);

        HAL_IWDG_Refresh((IWDG_HandleTypeDef *) mcu->handles.wdt);

        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    }
}

static void sensorsJob(void *arg) {
    McuDef *mcu = (McuDef *) arg;

    BaseType_t result = pdFALSE;
    uint32_t notifyValue = 0;
    TickType_t lastWakeTime = xTaskGetTickCount();
    TickType_t period = pdMS_TO_TICKS(100); // ms

    HAL_TIM_Base_Start((TIM_HandleTypeDef *) mcu->adc.timer.handle);
    HAL_ADC_Start_DMA((ADC_HandleTypeDef *) mcu->adc.handle, mcu->adc.rawValues, NUMBER_ADC_CHANNELS);

    while (1) {
        vTaskDelayUntil(&lastWakeTime, period);

        result = xTaskNotifyWait(0x00, ULONG_MAX, &notifyValue, pdMS_TO_TICKS(TIME_DELAY_MS));
        if (result == pdTRUE) {
            if (notifyValue & JOB_NOTIFY_ADC_FLAG) {
                uint32_t value = 0;
                for (size_t i = 0; i < (NUMBER_ADC_CHANNELS - 1); ++i) {
                    value = mcu->adc.rawValues[i];
                    value = (value * VDD_VALUE) >> 12;
                    mcu->adc.values[i] = (uint16_t) value;
                }
                mcu->temp = __HAL_ADC_CALC_TEMPERATURE(VDD_VALUE, mcu->adc.rawValues[NUMBER_ADC_CHANNELS - 1],
                                                       ADC_RESOLUTION_12B);
            }
            if (result & JOB_NOTIFY_ADC_ERR_FLAG) {
                mcu->adc.errType = HAL_ADC_GetError((ADC_HandleTypeDef *) mcu->adc.handle);
                mcu->adc.errors++;
            }
        } else {
            mcu->jobs.errors++;
        }
    }
}

static void communicationJob(void *arg) {
    McuDef *mcu = (McuDef *) arg;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int createJobs(McuDef *mcu) {
    mcu->jobs.handles[ROUTINE_JOB] = xTaskCreateStatic(routineJob, "routine", configMINIMAL_STACK_SIZE, (void *) mcu,
                                                       tskIDLE_PRIORITY + 1, task1Stack, &task1TCB);
    mcu->jobs.handles[SENSORS_JOB] = xTaskCreateStatic(sensorsJob, "sensors", configMINIMAL_STACK_SIZE, (void *) mcu,
                                                       tskIDLE_PRIORITY + 2, task2Stack, &task2TCB);
    mcu->jobs.handles[COMMUNICATION_JOB] = xTaskCreateStatic(communicationJob, "communication",
                                                             configMINIMAL_STACK_SIZE, (void *) mcu,
                                                             tskIDLE_PRIORITY + 3, task3Stack, &task3TCB);
    return 0;
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &idleTCB;
    *ppxIdleTaskStackBuffer = idleStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    *ppxTimerTaskTCBBuffer = &timerTCB;
    *ppxTimerTaskStackBuffer = timerStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationTickHook(void) {
    // Call it every FreeRTOS tick
    HAL_IncTick();
}

void vApplicationStackOverflowHook(void) {
    __disable_irq();

    while (1) {
    }
}

void VApplicationMallocFailedHook(void) {
    __disable_irq();

    while (1) {
    }
}
