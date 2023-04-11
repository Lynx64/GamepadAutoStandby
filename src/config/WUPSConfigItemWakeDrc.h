#include <wups.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ConfigItemWakeDrc {
    char *configId;
    WUPSConfigItemHandle handle;
    uint32_t defaultValue;
    uint32_t value;
    void *callback;
} ConfigItemWakeDrc;

typedef void (*WakeDrcValueChangedCallback)(ConfigItemWakeDrc *, uint32_t);

bool WUPSConfigItemWakeDrcAddToCategory(WUPSConfigCategoryHandle cat, const char *configId, const char *displayName, WakeDrcValueChangedCallback callback);

#define WUPSConfigItemWakeDrc_AddToCategoryHandled(__config__, __cat__, __configID__, __displayName__, __callback__) \
    do {                                                                                                                                                \
        if (!WUPSConfigItemWakeDrcAddToCategory(__cat__, __configID__, __displayName__, __callback__)) {             \
            WUPSConfig_Destroy(__config__);                                                                                                             \
            return 0;                                                                                                                                   \
        }                                                                                                                                               \
    } while (0)

#ifdef __cplusplus
}
#endif
