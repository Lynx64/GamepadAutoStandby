#pragma once

#include <wups.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PowerOffOnDrcValue {
    POWER_OFF_ON_DRC_READY_FOR_OFF,
    POWER_OFF_ON_DRC_OFF_SUCCESS,
    POWER_OFF_ON_DRC_READY_FOR_ON,
    POWER_OFF_ON_DRC_ON_SUCCESS,
    POWER_OFF_ON_DRC_ON_FAILED,
} PowerOffOnDrcValue;

typedef struct ConfigItemPowerOffOnDrc {
    const char *identifier;
    WUPSConfigItemHandle handle;
    PowerOffOnDrcValue defaultValue;
    PowerOffOnDrcValue valueAtCreation;
    PowerOffOnDrcValue value;
    void *valueChangedCallback;
} ConfigItemPowerOffOnDrc;

typedef void (*PowerOffOnDrcValueChangedCallback)(ConfigItemPowerOffOnDrc *, PowerOffOnDrcValue);

WUPSConfigAPIStatus
WUPSConfigItemPowerOffOnDrc_Create(const char *identifier,
                                   const char *displayName,
                                   PowerOffOnDrcValueChangedCallback callback,
                                   WUPSConfigItemHandle *outHandle);

WUPSConfigAPIStatus
WUPSConfigItemPowerOffOnDrc_AddToCategory(WUPSConfigCategoryHandle cat,
                                          const char *identifier,
                                          const char *displayName,
                                          PowerOffOnDrcValueChangedCallback callback);

#ifdef __cplusplus
}
#endif
