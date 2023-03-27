#include "config.h"
#include "common.h"
#include "retain_vars.hpp"
#include <wups.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemIntegerRange.h>
#include <wups/config/WUPSConfigItemMultipleValues.h>
#include <coreinit/time.h>
#include <nsysccr/cdc.h>
#include <string_view>

static bool sShutdownNow = false;

WUPS_USE_STORAGE("gamepad_auto_standby");

void initConfig()
{
    // Open storage to read values
    WUPSStorageError storageRes = WUPS_OpenStorage();
    if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
        //failed to open storage
    } else {
        // Try to get value from storage
        if ((storageRes = WUPS_GetInt(nullptr, "onIdleMode", reinterpret_cast<int32_t *>(&gOnIdleMode))) == WUPS_STORAGE_ERROR_NOT_FOUND) {
            // Add the value to the storage if it's missing
            if (WUPS_StoreInt(nullptr, "onIdleMode", (int32_t) gOnIdleMode) != WUPS_STORAGE_ERROR_SUCCESS) {
                //failed to store value
            }
        } else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
            //failed to get value
        }

        // Try to get value from storage
        if ((storageRes = WUPS_GetInt(nullptr, "standbyDelay", reinterpret_cast<int32_t *>(&gStandbyDelay))) == WUPS_STORAGE_ERROR_NOT_FOUND) {
            // Add the value to the storage if it's missing
            if (WUPS_StoreInt(nullptr, "standbyDelay", (int32_t) gStandbyDelay) != WUPS_STORAGE_ERROR_SUCCESS) {
                //failed to store value
            }
        } else if (storageRes != WUPS_STORAGE_ERROR_SUCCESS) {
            //failed to get value
        }

        // Close storage
        if (WUPS_CloseStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
            //failed to close storage
        }
    }

    if (gStandbyDelay < 1) {
        gStandbyDelay = 1;
    } else if (gStandbyDelay > 60) {
        gStandbyDelay = 60;
    }
    gStandbyDelayTicks = OSSecondsToTicks(gStandbyDelay * 60);
}

void boolItemCallback(ConfigItemBoolean *item, bool newValue) {
    if (item && item->configId) {
        //new value changed
        if (std::string_view(item->configId) == "shutdownNow") {
            sShutdownNow = newValue;
        }
    }
}

void multipleValueItemCallback(ConfigItemMultipleValues *item, uint32_t newValue) 
{
    if (item && item->configId) {
        //new value changed
        if (std::string_view(item->configId) == "onIdleMode") {
            gOnIdleMode = (OnIdleModeEnum) newValue;

            if (gOnIdleMode >= 3) {
                gOnIdleMode = ON_IDLE_NOTHING;
            }

            WUPS_StoreInt(nullptr, item->configId, (int32_t) gOnIdleMode);
        }
    }
}

void integerRangeItemCallback(ConfigItemIntegerRange *item, int32_t newValue) 
{
    if (item && item->configId) {
        //new value changed
        if (std::string_view(item->configId) == "standbyDelay") {
            gStandbyDelay = newValue;

            if (gStandbyDelay < 1) {
                gStandbyDelay = 1;
            } else if (gStandbyDelay > 60) {
                gStandbyDelay = 60;
            }
            
            WUPS_StoreInt(nullptr, item->configId, (int32_t) gStandbyDelay);
        }
    }
}

WUPS_GET_CONFIG()
{
    // We open the storage, so we can persist the configuration the user did
    if (WUPS_OpenStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        //failed to open storage
        return 0;
    }

    WUPSConfigHandle config;
    WUPSConfig_CreateHandled(&config, "Gamepad Auto Standby");

    WUPSConfigCategoryHandle setting;
    WUPSConfig_AddCategoryByNameHandled(config, "Settings", &setting);

    ConfigItemMultipleValuesPair mode[3];
    mode[0].value       = ON_IDLE_NOTHING;
    mode[0].valueName   = (char *) "Do nothing";

    mode[1].value       = ON_IDLE_STANDBY;
    mode[1].valueName   = (char *) "Standby";

    mode[2].value       = ON_IDLE_SHUTDOWN;
    mode[2].valueName   = (char *) "Power off";

    uint32_t defaultIndex = 0;
    uint32_t curIndex = 0;
    for (auto &cur : mode) {
        if (cur.value == gOnIdleMode) {
            defaultIndex = curIndex;
            break;
        }
        curIndex++;
    }
    //you could also get the DRC state, but I don't know what all the possible states are/what they mean, and this code is more simple anyway and seems to get the job done
    if (CCRCDCDevicePing(CCR_CDC_DESTINATION_DRC0) == 0) {
        WUPSConfigItemBoolean_AddToCategoryHandled(config, setting, "shutdownNow", "Power off Gamepad now", sShutdownNow, &boolItemCallback);
    }

    WUPSConfigItemMultipleValues_AddToCategoryHandled(config, setting, "onIdleMode", "On idle", defaultIndex, mode,
                                                      sizeof(mode) / sizeof(mode[0]), &multipleValueItemCallback);

    WUPSConfigItemIntegerRange_AddToCategoryHandled(config, setting, "standbyDelay", "Delay (minutes)", gStandbyDelay, 1, 60, &integerRangeItemCallback);

    return config;
}

WUPS_CONFIG_CLOSED()
{
    // Save all changes
    if (WUPS_CloseStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        //failed to close storage
    }

    gStandbyDelayTicks = OSSecondsToTicks(gStandbyDelay * 60);
    if (sShutdownNow) {
        CCRCDCSysConsoleShutdownInd(CCR_CDC_DESTINATION_DRC0);
        gInShutdown = true;
        sShutdownNow = false;
    }
}