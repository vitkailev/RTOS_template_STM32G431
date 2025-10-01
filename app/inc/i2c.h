#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

enum I2C_Errors {
    I2C_SUCCESS = 0,
    I2C_NOT_INIT = -1,
    I2C_WRONG_DATA = -2,
    I2C_HW_ERROR = -3,

    I2C_NUMBER_ERRORS = 3
};

typedef struct I2CDef I2CDef;

typedef int32_t (*I2CFun_update)(I2CDef *i2c);

typedef int32_t (*I2CFun_tx)(I2CDef *i2c, uint16_t addr, const void *src, size_t size, bool isNeedStop);

typedef int32_t (*I2CFun_rx)(I2CDef *i2c, uint16_t addr, void *dst, size_t size);

typedef int32_t (*I2CFun_state)(const I2CDef *i2c);

struct I2CDef {
    void *const handle;

    bool isInit;
    bool abort;
    int32_t errType;
    int32_t errors;

    const I2CFun_update init;
    const I2CFun_tx sendData;
    const I2CFun_rx readData;
    const I2CFun_update saveError;
    const I2CFun_state getErrorType;
    const I2CFun_state getNumOfErrors;
    const I2CFun_state isFailed;
};

#ifdef __cplusplus
}
#endif

#endif //I2C_H
