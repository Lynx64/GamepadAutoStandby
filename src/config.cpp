#include "config.h"
#include "common.h"
#include "retain_vars.hpp"
#include "config/WUPSConfigItemPowerOffOnDrc.h"
#include <wups.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemIntegerRange.h>
#include <wups/config/WUPSConfigItemMultipleValues.h>
#include <coreinit/time.h>
#include <string_view>

WUPS_USE_STORAGE("gamepad_auto_standby");

void boolItemCallback(ConfigItemBoolean *item, bool newValue) 
{
    if (item && item->identifier) {
        //new value changed
        if (std::string_view(WII_FORWARDER_TV_ONLY_CONFIG_ID) == item->identifier) {
            gWiiForwarderTVOnly = newValue;
            WUPSStorageAPI_StoreBool(nullptr, item->identifier, gWiiForwarderTVOnly);
        } else if (std::string_view(KEEP_ON_WHILE_CHARGING_CONFIG_ID) == item->identifier) {
            gKeepOnWhileCharging = newValue;
            WUPSStorageAPI_StoreBool(nullptr, item->identifier, gKeepOnWhileCharging);
        } else if (std::string_view(SHUTDOWN_ON_BOOT_CONFIG_ID) == item->identifier) {
            gShutdownOnBoot = newValue;
            WUPSStorageAPI_StoreBool(nullptr, item->identifier, gShutdownOnBoot);
        }
    }
}

void multipleValueItemCallback(ConfigItemMultipleValues *item, uint32_t newValue) 
{
    if (item && item->identifier) {
        //new value changed
        if (std::string_view(ON_IDLE_MODE_CONFIG_ID) == item->identifier) {
            gOnIdleMode = newValue;
            WUPSStorageAPI_StoreInt(nullptr, item->identifier, gOnIdleMode);
        }
    }
}

void integerRangeItemCallback(ConfigItemIntegerRange *item, int32_t newValue) 
{
    if (item && item->identifier) {
        //new value changed
        if (std::string_view(STANDBY_DELAY_CONFIG_ID) == item->identifier) {
            gStandbyDelay = newValue;

            if (gStandbyDelay < 1) {
                gStandbyDelay = 1;
            } else if (gStandbyDelay > 60) {
                gStandbyDelay = 60;
            }
            gStandbyDelayTicks = OSSecondsToTicks(gStandbyDelay * 60);

            WUPSStorageAPI_StoreInt(nullptr, item->identifier, gStandbyDelay);
        }
    }
}

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle rootHandle)
{
    // Category: Root
    if (WUPSConfigItemPowerOffOnDrc_AddToCategory(rootHandle, nullptr, "Power off/on GamePad", nullptr) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

    // Category: Settings
    WUPSConfigCategoryHandle settings;
    WUPSConfigAPICreateCategoryOptionsV1 settingsOptions = {.name = "Settings"};
    if (WUPSConfigAPI_Category_Create(settingsOptions, &settings) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

    ConfigItemMultipleValuesPair mode[3];
    mode[0].value       = ON_IDLE_NOTHING;
    mode[0].valueName   = "Do nothing";

    mode[1].value       = ON_IDLE_STANDBY;
    mode[1].valueName   = "Standby";

    mode[2].value       = ON_IDLE_SHUTDOWN;
    mode[2].valueName   = "Power off";

    int32_t currentValueIndex = 0;
    int32_t index = 0;
    for (const auto &cur : mode) {
        if (cur.value == gOnIdleMode) {
            currentValueIndex = index;
            break;
        }
        index++;
    }

    if (WUPSConfigItemMultipleValues_AddToCategory(settings, ON_IDLE_MODE_CONFIG_ID, "On idle", 1, currentValueIndex, mode, 3, &multipleValueItemCallback) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

    if (WUPSConfigItemIntegerRange_AddToCategory(settings, STANDBY_DELAY_CONFIG_ID, "Delay (minutes)", DEFAULT_STANDBY_DELAY_VALUE, gStandbyDelay, 1, 60, &integerRangeItemCallback) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

    if (WUPSConfigItemBoolean_AddToCategory(settings, SHUTDOWN_ON_BOOT_CONFIG_ID, "Power off on boot", DEFAULT_SHUTDOWN_ON_BOOT_VALUE, gShutdownOnBoot, &boolItemCallback) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

    if (WUPSConfigItemBoolean_AddToCategory(settings, KEEP_ON_WHILE_CHARGING_CONFIG_ID, "Keep on while charging", DEFAULT_KEEP_ON_WHILE_CHARGING_VALUE, gKeepOnWhileCharging, &boolItemCallback) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

    if (WUPSConfigItemBoolean_AddToCategory(settings, WII_FORWARDER_TV_ONLY_CONFIG_ID, "Launch .wuhb vWii forwarders on TV only", DEFAULT_WII_FORWARDER_TV_ONLY_VALUE, gWiiForwarderTVOnly, &boolItemCallback) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;

    if (WUPSConfigAPI_Category_AddCategory(rootHandle, settings) != WUPSCONFIG_API_RESULT_SUCCESS)
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void ConfigMenuClosedCallback()
{
    // Save all changes
    WUPSStorageAPI_SaveStorage(false);
}

void initConfig()
{
    WUPSConfigAPIOptionsV1 configOptions = {.name = "GamePad Auto Standby"};
    WUPSConfigAPI_Init(configOptions, ConfigMenuOpenedCallback, ConfigMenuClosedCallback);

    // Try to get value from storage
    if (WUPSStorageAPI_GetInt(nullptr, ON_IDLE_MODE_CONFIG_ID, &gOnIdleMode) == WUPS_STORAGE_ERROR_NOT_FOUND) {
        // Add the value to the storage if it's missing
        WUPSStorageAPI_StoreInt(nullptr, ON_IDLE_MODE_CONFIG_ID, gOnIdleMode);
    }

    if (WUPSStorageAPI_GetInt(nullptr, STANDBY_DELAY_CONFIG_ID, &gStandbyDelay) == WUPS_STORAGE_ERROR_NOT_FOUND) {
        WUPSStorageAPI_StoreInt(nullptr, STANDBY_DELAY_CONFIG_ID, gStandbyDelay);
    }

    if (WUPSStorageAPI_GetBool(nullptr, WII_FORWARDER_TV_ONLY_CONFIG_ID, &gWiiForwarderTVOnly) == WUPS_STORAGE_ERROR_NOT_FOUND) {
        WUPSStorageAPI_StoreBool(nullptr, WII_FORWARDER_TV_ONLY_CONFIG_ID, gWiiForwarderTVOnly);
    }

    if (WUPSStorageAPI_GetBool(nullptr, KEEP_ON_WHILE_CHARGING_CONFIG_ID, &gKeepOnWhileCharging) == WUPS_STORAGE_ERROR_NOT_FOUND) {
        WUPSStorageAPI_StoreBool(nullptr, KEEP_ON_WHILE_CHARGING_CONFIG_ID, gKeepOnWhileCharging);
    }

    if (WUPSStorageAPI_GetBool(nullptr, SHUTDOWN_ON_BOOT_CONFIG_ID, &gShutdownOnBoot) == WUPS_STORAGE_ERROR_NOT_FOUND) {
        WUPSStorageAPI_StoreBool(nullptr, SHUTDOWN_ON_BOOT_CONFIG_ID, gShutdownOnBoot);
    }

    // Save storage
    WUPSStorageAPI_SaveStorage(false);

    if (gStandbyDelay < 1) {
        gStandbyDelay = 1;
    } else if (gStandbyDelay > 60) {
        gStandbyDelay = 60;
    }
    gStandbyDelayTicks = OSSecondsToTicks(gStandbyDelay * 60);
}
