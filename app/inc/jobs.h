#ifndef JOBS_H
#define JOBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "variables.h"
#include "SerialJob.h"
#include "I2CBusJob.h"

enum Job_Notifications {
    JOB_NOTIF_SENSOR_FLAG = 1 << 0,
    JOB_NOTIF_SENSOR_ERR_FLAG = 1 << 1,
};

enum Job_Constants {
    ROUTINE_JOB = 0,
    SENSORS_JOB,
    COMMUNICATION_JOB,
    SERIAL_PORT_JOB,
    I2CBUS_JOB,
    SERVICE_JOB,
    NUMBER_JOBS,

    ROUTINE_DELAY_MS = 20,
    SENSORS_DELAY_MS = 20,
    SENSORS_NOTIF_DELAY_MS = 50,
    COMMUNICATION_DELAY_MS = 100,
    SERVICE_DELAY_MS = 100,
};

typedef struct {
    uint32_t errors; // something expired, if xTicksToWait != portMAX_DELAY

    TaskHandle_t handles[NUMBER_JOBS];

    McuDef hardware;
} JobsDef;

typedef struct {
    I2CBusDef interface;
} SensorsDef;

extern JobsDef Application;
extern SerialPortDef Serial;
extern SensorsDef Sensors;

int createJobs(JobsDef *jobs);

#ifdef __cplusplus
}
#endif

#endif //JOBS_H
