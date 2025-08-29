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
};

typedef struct UartDef UartDef;

typedef int32_t (*UartFun_Update)(UartDef *uart);

typedef int32_t (*UartFun_Tx)(UartDef *uart, const void *data, size_t size);

typedef int32_t (*UartFun_Rx)(UartDef *uart, void *data, size_t size);

typedef int32_t (*UartFun_State)(const UartDef *uart);

struct UartDef {
    void *const handle;

    bool isInit;
    enum UART_Errors errType;
    int32_t errors;

    const UartFun_Update init;
    const UartFun_Tx sendData;
    const UartFun_Rx readData;
    const UartFun_Update saveError;
    const UartFun_State getErrorType;
    const UartFun_State getNumOfErrors;
};

#ifdef __cplusplus
}
#endif

#endif //UART_H
