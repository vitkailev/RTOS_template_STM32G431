#ifndef JOBS_H
#define JOBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "variables.h"

enum Job_Constants {
    JOB_NOTIFY_ADC_FLAG = 1 << 0,
    JOB_NOTIFY_ADC_ERR_FLAG = 1 << 1,
};

int createJobs(McuDef *mcu);

#ifdef __cplusplus
}
#endif

#endif //JOBS_H
