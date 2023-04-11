#include "WUPSConfigItemCheckbox.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <wups.h>

int32_t WUPSConfigItemCheckbox_getCurrentValueDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemCheckbox *) context;
    snprintf(out_buf, out_size, "%s", item->value ? item->trueValue : item->falseValue);
    return 0;
}

bool WUPSConfigItemCheckbox_callCallback(void *context) {
    auto *item = (ConfigItemCheckbox *) context;
    if (item->callback != nullptr) {
        ((CheckboxValueChangedCallback) (item->callback))(item, item->value);
        return true;
    }
    return false;
}

void WUPSConfigItemCheckbox_onButtonPressed(void *context, WUPSConfigButtons buttons) {
    auto *item = (ConfigItemCheckbox *) context;
    if ((buttons & WUPS_CONFIG_BUTTON_A) || (buttons & WUPS_CONFIG_BUTTON_LEFT) || (buttons & WUPS_CONFIG_BUTTON_RIGHT)) {
        item->value = !item->value;
    }
}

bool WUPSConfigItemCheckbox_isMovementAllowed(void *context) {
    return true;
}

int32_t WUPSConfigItemCheckbox_getCurrentValueSelectedDisplay(void *context, char *out_buf, int32_t out_size) {
    auto *item = (ConfigItemCheckbox *) context;
    if (item->value) {
        snprintf(out_buf, out_size, "(Exit to apply) %s", item->trueValue);
    } else {
        snprintf(out_buf, out_size, "%s", item->falseValue);
    }
    return 0;
}

void WUPSConfigItemCheckbox_restoreDefault(void *context) {
    auto *item  = (ConfigItemCheckbox *) context;
    item->value = item->defaultValue;
}

void WUPSConfigItemCheckbox_onSelected(void *context, bool isSelected) {
}

void WUPSConfigItemCheckbox_onDelete(void *context) {
    auto *item = (ConfigItemCheckbox *) context;
    free(item->configId);
    free(item);
}

extern "C" bool
WUPSConfigItemCheckboxAddToCategory(WUPSConfigCategoryHandle cat, const char *configID, const char *displayName, bool defaultValue, CheckboxValueChangedCallback callback) {
    if (cat == 0) {
        return false;
    }
    auto *item = (ConfigItemCheckbox *) malloc(sizeof(ConfigItemCheckbox));
    if (item == nullptr) {
        return false;
    }

    if (configID != nullptr) {
        item->configId = strdup(configID);
    } else {
        item->configId = nullptr;
    }

    item->defaultValue                = defaultValue;
    item->value                       = defaultValue;
    item->callback                    = (void *) callback;
    snprintf(item->trueValue, sizeof(item->trueValue), "%s", "\u25C9");
    snprintf(item->falseValue, sizeof(item->falseValue), "%s", "\u25CB");

    //"\u2588\u25A0\u25C6\u25C9\u25CF\uE010\uE01D\uE01F\uE028\uE03E \uE03F\uE040\uE071\uE078\uE098"
    //"\u20DD\u25A1\u25A2\u25C7\u25CB\u25CE\u25EF\u30ED\uE070"

    WUPSConfigCallbacks_t callbacks = {
            .getCurrentValueDisplay         = &WUPSConfigItemCheckbox_getCurrentValueDisplay,
            .getCurrentValueSelectedDisplay = &WUPSConfigItemCheckbox_getCurrentValueSelectedDisplay,
            .onSelected                     = &WUPSConfigItemCheckbox_onSelected,
            .restoreDefault                 = &WUPSConfigItemCheckbox_restoreDefault,
            .isMovementAllowed              = &WUPSConfigItemCheckbox_isMovementAllowed,
            .callCallback                   = &WUPSConfigItemCheckbox_callCallback,
            .onButtonPressed                = &WUPSConfigItemCheckbox_onButtonPressed,
            .onDelete                       = &WUPSConfigItemCheckbox_onDelete};

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
