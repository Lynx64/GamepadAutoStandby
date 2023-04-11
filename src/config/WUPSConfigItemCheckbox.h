#include <wups.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ConfigItemCheckbox {
    char *configId;
    WUPSConfigItemHandle handle;
    bool defaultValue;
    bool value;
    char trueValue[32];
    char falseValue[32];
    void *callback;
} ConfigItemCheckbox;

typedef void (*CheckboxValueChangedCallback)(ConfigItemCheckbox *, bool);

bool WUPSConfigItemCheckboxAddToCategory(WUPSConfigCategoryHandle cat, const char *configId, const char *displayName, bool defaultValue, CheckboxValueChangedCallback callback);

#define WUPSConfigItemCheckbox_AddToCategoryHandled(__config__, __cat__, __configID__, __displayName__, __defaultValue__, __callback__) \
    do {                                                                                                                                                \
        if (!WUPSConfigItemCheckboxAddToCategory(__cat__, __configID__, __displayName__, __defaultValue__, __callback__)) {             \
            WUPSConfig_Destroy(__config__);                                                                                                             \
            return 0;                                                                                                                                   \
        }                                                                                                                                               \
    } while (0)

#ifdef __cplusplus
}
#endif
