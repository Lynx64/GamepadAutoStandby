#include "retain_vars.hpp"
#include "common.h"
#include <vpad/input.h>
#include <wups.h>
#include <coreinit/time.h>
#include <nn/ccr/sys.h>

static OSTime sLastInputGamepad = OSGetSystemTime();
static bool sInStandby = false;

extern "C" uint32_t VPADGetButtonProcMode(VPADChan chan);
DECL_FUNCTION(int32_t, VPADRead, VPADChan chan, VPADStatus *buffer, uint32_t buffer_size, VPADReadError *error) 
{
    VPADReadError realError;
    int32_t result = real_VPADRead(chan, buffer, buffer_size, &realError);
    uint32_t end = 1;
    if (VPADGetButtonProcMode(chan) == 1) {
        end = buffer_size;
    }

    if (result > 0 && realError == VPAD_READ_SUCCESS) {
        for (uint32_t i = 0; i < end; i++) {
            if (buffer[i].trigger > 0 || buffer[i].tpNormal.touched > 0) {
                sLastInputGamepad = OSGetSystemTime();
                break;
            }
        }
        if (sInStandby) {
            sLastInputGamepad = OSGetSystemTime();
            sInStandby = false;
        } else if (gOnIdleMode != ON_IDLE_NOTHING && sLastInputGamepad + gStandbyDelayTicks < OSGetSystemTime()) {
            if (gOnIdleMode == ON_IDLE_STANDBY) {
                VPADLcdMode lcdMode;
                VPADGetLcdMode(VPAD_CHAN_0, &lcdMode);
                if (lcdMode != VPAD_LCD_OFF) {
                    VPADSetLcdMode(VPAD_CHAN_0, VPAD_LCD_STANDBY);
                }
            } else if (gOnIdleMode == ON_IDLE_SHUTDOWN) {
                CCRSysDRCShutdown();
            }
            sInStandby = true;
        }
    }

    if (error) {
        *error = realError;
    }
    return result;
}

WUPS_MUST_REPLACE(VPADRead, WUPS_LOADER_LIBRARY_VPAD, VPADRead);
