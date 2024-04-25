#include "WUPSConfigItemPowerOffOnDrc.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <wups.h>
#include <coreinit/time.h>
#include <nsysccr/cdc.h>

static int32_t WUPSConfigItemPowerOffOnDrc_getCurrentValueDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemPowerOffOnDrc *) context;
    if (item->value == POWER_OFF_ON_DRC_OFF_SUCCESS || item->value == POWER_OFF_ON_DRC_ON_SUCCESS) {
        snprintf(out_buf, out_size, "\u25C9");
    } else {
        snprintf(out_buf, out_size, "\u25CB");
    }
    return 0;
}

static void WUPSConfigItemPowerOffOnDrc_onCloseCallback(void *context) {
    auto *item = (ConfigItemPowerOffOnDrc *) context;
    if (item->valueAtCreation != item->value && item->valueChangedCallback != nullptr) {
        ((PowerOffOnDrcValueChangedCallback) (item->valueChangedCallback))(item, item->value);
    }
}

static void WUPSConfigItemPowerOffOnDrc_onInput(void *context, WUPSConfigSimplePadData input) {
    auto *item = (ConfigItemPowerOffOnDrc *) context;
    if (input.buttons_d & WUPS_CONFIG_BUTTON_A) {
        if (item->value == POWER_OFF_ON_DRC_READY_FOR_OFF) {
            CCRCDCSysConsoleShutdownInd(CCR_CDC_DESTINATION_DRC0);
            item->value = POWER_OFF_ON_DRC_OFF_SUCCESS;
        } else if (item->value == POWER_OFF_ON_DRC_READY_FOR_ON || item->value == POWER_OFF_ON_DRC_ON_FAILED) {
            CCRCDCWowlWakeDrcArg wakeDrcArg = {.state = CCR_CDC_WAKE_STATE_ACTIVE};
            OSTime timeout = OSGetSystemTime() + OSMillisecondsToTicks(830);
            CCRCDCWowlWakeDrc(&wakeDrcArg);
            if (OSGetSystemTime() < timeout) {
                item->value = POWER_OFF_ON_DRC_ON_SUCCESS;
            } else {
                item->value = POWER_OFF_ON_DRC_ON_FAILED;
            }
        }
    }
}

static int32_t WUPSConfigItemPowerOffOnDrc_getCurrentValueSelectedDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemPowerOffOnDrc *) context;
    switch (item->value)
    {
    case POWER_OFF_ON_DRC_READY_FOR_OFF:
        snprintf(out_buf, out_size, "Power OFF \uE000");
        break;
    case POWER_OFF_ON_DRC_OFF_SUCCESS:
    case POWER_OFF_ON_DRC_ON_SUCCESS:
        snprintf(out_buf, out_size, "\u25C9");
        break;
    case POWER_OFF_ON_DRC_READY_FOR_ON:
        snprintf(out_buf, out_size, "Power ON \uE000");
        break;
    case POWER_OFF_ON_DRC_ON_FAILED:
        snprintf(out_buf, out_size, "Power ON failed \uE08E\uE000");
        break;
    default:
        snprintf(out_buf, out_size, "(Invalid value)");
        break;
    }
    return 0;
}

static void WUPSConfigItemPowerOffOnDrc_restoreDefault(void *context) {
    auto *item  = (ConfigItemPowerOffOnDrc *) context;
    item->value = item->defaultValue;
}

static void WUPSConfigItemPowerOffOnDrc_onSelected(void *context, bool isSelected) {
    auto *item = (ConfigItemPowerOffOnDrc *) context;
    if (isSelected) {
        if (CCRCDCDevicePing(CCR_CDC_DESTINATION_DRC0) == 0) {
            item->value = POWER_OFF_ON_DRC_READY_FOR_OFF;
        } else {
            item->value = POWER_OFF_ON_DRC_READY_FOR_ON;
        }
    }
}

static void WUPSConfigItemPowerOffOnDrc_Cleanup(ConfigItemPowerOffOnDrc *item) {
    if (!item) {
        return;
    }
    free((void *) item->identifier);
    free(item);
}

static void WUPSConfigItemPowerOffOnDrc_onDelete(void *context) {
    WUPSConfigItemPowerOffOnDrc_Cleanup((ConfigItemPowerOffOnDrc *) context);
}

extern "C" WUPSConfigAPIStatus
WUPSConfigItemPowerOffOnDrc_Create(const char *identifier,
                                   const char *displayName,
                                   PowerOffOnDrcValueChangedCallback callback,
                                   WUPSConfigItemHandle *outHandle) {
    if (outHandle == nullptr) {
        return WUPSCONFIG_API_RESULT_INVALID_ARGUMENT;
    }
    auto *item = (ConfigItemPowerOffOnDrc *) malloc(sizeof(ConfigItemPowerOffOnDrc));
    if (item == nullptr) {
        return WUPSCONFIG_API_RESULT_OUT_OF_MEMORY;
    }

    if (identifier != nullptr) {
        item->identifier = strdup(identifier);
    } else {
        item->identifier = nullptr;
    }

    item->defaultValue         = POWER_OFF_ON_DRC_READY_FOR_OFF;
    item->value                = POWER_OFF_ON_DRC_READY_FOR_OFF;
    item->valueAtCreation      = POWER_OFF_ON_DRC_READY_FOR_OFF;
    item->valueChangedCallback = (void *) callback;

    WUPSConfigAPIItemCallbacksV2 callbacks = {
            .getCurrentValueDisplay         = &WUPSConfigItemPowerOffOnDrc_getCurrentValueDisplay,
            .getCurrentValueSelectedDisplay = &WUPSConfigItemPowerOffOnDrc_getCurrentValueSelectedDisplay,
            .onSelected                     = &WUPSConfigItemPowerOffOnDrc_onSelected,
            .restoreDefault                 = &WUPSConfigItemPowerOffOnDrc_restoreDefault,
            .isMovementAllowed              = nullptr,
            .onCloseCallback                = &WUPSConfigItemPowerOffOnDrc_onCloseCallback,
            .onInput                        = &WUPSConfigItemPowerOffOnDrc_onInput,
            .onInputEx                      = nullptr,
            .onDelete                       = &WUPSConfigItemPowerOffOnDrc_onDelete};

    WUPSConfigAPIItemOptionsV2 options = {
            .displayName = displayName,
            .context     = item,
            .callbacks   = callbacks};

    WUPSConfigAPIStatus err;
    if ((err = WUPSConfigAPI_Item_Create(options, &item->handle)) != WUPSCONFIG_API_RESULT_SUCCESS) {
        WUPSConfigItemPowerOffOnDrc_Cleanup(item);
        return err;
    }

    *outHandle = item->handle;
    return WUPSCONFIG_API_RESULT_SUCCESS;
}

extern "C" WUPSConfigAPIStatus
WUPSConfigItemPowerOffOnDrc_AddToCategory(WUPSConfigCategoryHandle cat,
                                          const char *identifier,
                                          const char *displayName,
                                          PowerOffOnDrcValueChangedCallback callback) {
    WUPSConfigItemHandle itemHandle;
    WUPSConfigAPIStatus res;
    if ((res = WUPSConfigItemPowerOffOnDrc_Create(identifier,
                                                  displayName,
                                                  callback,
                                                  &itemHandle)) != WUPSCONFIG_API_RESULT_SUCCESS) {
        return res;
    }

    if ((res = WUPSConfigAPI_Category_AddItem(cat, itemHandle)) != WUPSCONFIG_API_RESULT_SUCCESS) {
        WUPSConfigAPI_Item_Destroy(itemHandle);
        return res;
    }
    return WUPSCONFIG_API_RESULT_SUCCESS;
}
