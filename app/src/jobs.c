#include "stm32g4xx_hal.h"

#include "FreeRTOS.h"

#include "jobs.h"

static StaticTask_t idleTCB;
static StackType_t idleStack[configMINIMAL_STACK_SIZE];
static StaticTask_t timerTCB;
static StackType_t timerStack[configTIMER_TASK_STACK_DEPTH];

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
