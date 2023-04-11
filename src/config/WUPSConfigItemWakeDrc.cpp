#include "WUPSConfigItemWakeDrc.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <wups.h>
#include <coreinit/time.h>
#include <nsysccr/cdc.h>

typedef struct CCRCDCWowlWakeDrcArgs CCRCDCWowlWakeDrcArgs;

struct WUT_PACKED CCRCDCWowlWakeDrcArgs
{
    WUT_PADDING_BYTES(6);
    //! Must be 1 or 2
    uint8_t arg;
};
WUT_CHECK_OFFSET(CCRCDCWowlWakeDrcArgs, 6, arg);
WUT_CHECK_SIZE(CCRCDCWowlWakeDrcArgs, 7);

/**
 * \return
 * 0 on success or timeout (i.e. out of range/no battery).
 * 0xFFE31B5B if DRC0 already connected.
 */
extern "C" int32_t CCRCDCWowlWakeDrc(CCRCDCWowlWakeDrcArgs *args);

int32_t WUPSConfigItemWakeDrc_getCurrentValueDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemWakeDrc *) context;
    if (item->value == 1) {
        snprintf(out_buf, out_size, "\u25C9");
    } else {
        snprintf(out_buf, out_size, "\u25CB");
    }
    return 0;
}

bool WUPSConfigItemWakeDrc_callCallback(void *context) {
    auto *item = (ConfigItemWakeDrc *) context;
    if (item->callback != nullptr) {
        ((WakeDrcValueChangedCallback) (item->callback))(item, item->value);
        return true;
    }
    return false;
}

void WUPSConfigItemWakeDrc_onButtonPressed(void *context, WUPSConfigButtons buttons) {
    auto *item = (ConfigItemWakeDrc *) context;
    if ((buttons & WUPS_CONFIG_BUTTON_A) == WUPS_CONFIG_BUTTON_A) {
        CCRCDCWowlWakeDrcArgs wakeDrcArgs = {.arg = 1};
        OSTime timeToWake = OSGetSystemTime();
        CCRCDCWowlWakeDrc(&wakeDrcArgs);
        if (OSGetSystemTime() < (timeToWake + (OSTime) OSMillisecondsToTicks(830))) {
            item->value = 1;
        } else {
            item->value = 2;
        }
    }
}

bool WUPSConfigItemWakeDrc_isMovementAllowed(void *context) {
    return true;
}

int32_t WUPSConfigItemWakeDrc_getCurrentValueSelectedDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemWakeDrc *) context;
    switch (item->value)
    {
    case 0:
        snprintf(out_buf, out_size, "\uE042");
        break;
    case 1:
        snprintf(out_buf, out_size, "(Connected) \u25C9");
        break;
    case 2:
        snprintf(out_buf, out_size, "(Failed to connect) \uE08E\uE042");
        break;
    default:
        snprintf(out_buf, out_size, "(Invalid value)");
        break;
    }
    return 0;
}

void WUPSConfigItemWakeDrc_restoreDefault(void *context) {
    auto *item  = (ConfigItemWakeDrc *) context;
    item->value = item->defaultValue;
}

void WUPSConfigItemWakeDrc_onSelected(void *context, bool isSelected) {
    auto *item = (ConfigItemWakeDrc *) context;
    if (isSelected) {
        if (CCRCDCDevicePing(CCR_CDC_DESTINATION_DRC0) == 0) {
            item->value = 1;
        } else {
            item->value = 0;
        }
    }
}

void WUPSConfigItemWakeDrc_onDelete(void *context) {
    auto *item = (ConfigItemWakeDrc *) context;
    free(item->configId);
    free(item);
}

extern "C" bool
WUPSConfigItemWakeDrcAddToCategory(WUPSConfigCategoryHandle cat, const char *configID, const char *displayName, WakeDrcValueChangedCallback callback) {
    if (cat == 0) {
        return false;
    }
    auto *item = (ConfigItemWakeDrc *) malloc(sizeof(ConfigItemWakeDrc));
    if (item == nullptr) {
        return false;
    }

    if (configID != nullptr) {
        item->configId = strdup(configID);
    } else {
        item->configId = nullptr;
    }

    item->defaultValue                = 0;
    item->value                       = 0;
    item->callback                    = (void *) callback;

    WUPSConfigCallbacks_t callbacks = {
            .getCurrentValueDisplay         = &WUPSConfigItemWakeDrc_getCurrentValueDisplay,
            .getCurrentValueSelectedDisplay = &WUPSConfigItemWakeDrc_getCurrentValueSelectedDisplay,
            .onSelected                     = &WUPSConfigItemWakeDrc_onSelected,
            .restoreDefault                 = &WUPSConfigItemWakeDrc_restoreDefault,
            .isMovementAllowed              = &WUPSConfigItemWakeDrc_isMovementAllowed,
            .callCallback                   = &WUPSConfigItemWakeDrc_callCallback,
            .onButtonPressed                = &WUPSConfigItemWakeDrc_onButtonPressed,
            .onDelete                       = &WUPSConfigItemWakeDrc_onDelete};

    if (WUPSConfigItem_Create(&item->handle, configID, displayName, callbacks, item) < 0) {
        free(item);
        return false;
    }

    if (WUPSConfigCategory_AddItem(cat, item->handle) < 0) {
        WUPSConfigItem_Destroy(item->handle);
        return false;
    }
    return true;
}
