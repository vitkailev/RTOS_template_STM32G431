#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

enum UART_Errors {
    UART_SUCCESS = 0,
    UART_NOT_INIT = -1,
    UART_WRONG_DATA = -2,
    UART_HW_ERROR = -3,

    UART_NUMBER_ERRORS = 3
};

typedef struct UartDef UartDef;

typedef int32_t (*UartFun_update)(UartDef *uart);

typedef int32_t (*UartFun_tx)(UartDef *uart, const void *data, size_t size);

typedef int32_t (*UartFun_rx)(UartDef *uart, void *data, size_t size);

typedef int32_t (*UartFun_state)(const UartDef *uart);

struct UartDef {
    void *const handle;

    bool isInit;
    int32_t errType;
    int32_t errors;

    const UartFun_update init;
    const UartFun_tx sendData;
    const UartFun_rx readData;
    const UartFun_update saveError;
    const UartFun_state getErrorType;
    const UartFun_state getNumOfErrors;
};

#ifdef __cplusplus
}
#endif

#endif //UART_H
