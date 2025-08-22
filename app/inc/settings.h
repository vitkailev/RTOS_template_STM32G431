#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "variables.h"

enum Settings_Constants {
    SETTING_SUCCESS = 0,
    SETTING_ERROR = -1,
};

int initialization(McuDef *mcu);

#ifdef __cplusplus
}
#endif

#endif // SETTINGS_H
