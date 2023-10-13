#include "retain_vars.hpp"
#include "common.h"
#include <vpad/input.h>
#include <wups.h>
#include <coreinit/time.h>
#include <nsysccr/cdc.h>
#include <nn/cmpt/cmpt.h>
#include <coreinit/title.h>
#include <sysapp/title.h>

extern "C" uint32_t VPADGetButtonProcMode(VPADChan chan);

static OSTime sLastInputGamepad = OSGetSystemTime();
static bool sInStandby = false;

DECL_FUNCTION(int32_t, VPADRead, VPADChan chan, VPADStatus *buffer, uint32_t bufferSize, VPADReadError *error)
{
    VPADReadError realError = VPAD_READ_UNINITIALIZED;
    int32_t result = real_VPADRead(chan, buffer, bufferSize, &realError);
    uint32_t end = 1;
    if (VPADGetButtonProcMode(chan) == 1) {
        end = bufferSize;
    }

    if (result > 0 && realError == VPAD_READ_SUCCESS) {

        if (gInShutdown) {

            sLastInputGamepad = OSGetSystemTime();
            gInShutdown = false;
            
        } else if (sInStandby) {

            VPADLcdMode lcdMode = VPAD_LCD_ON;
            VPADGetLcdMode(VPAD_CHAN_0, &lcdMode);
            if (lcdMode != VPAD_LCD_STANDBY) {
                sLastInputGamepad = OSGetSystemTime();
                sInStandby = false;
            }

        } else {

            for (uint32_t i = 0; i < end; i++) {
                if (buffer[i].trigger > 0 || buffer[i].tpNormal.touched > 0) {
                    sLastInputGamepad = OSGetSystemTime();
                    break;
                }
            }

            if (sLastInputGamepad + gStandbyDelayTicks < OSGetSystemTime()) {

                if (gOnIdleMode == ON_IDLE_STANDBY) {
                    VPADLcdMode lcdMode = VPAD_LCD_ON;
                    VPADGetLcdMode(VPAD_CHAN_0, &lcdMode);
                    if (lcdMode != VPAD_LCD_OFF && (!gKeepOnWhileCharging || buffer[0].battery != 0)) {
                        VPADSetLcdMode(VPAD_CHAN_0, VPAD_LCD_STANDBY);
                    }
                    sInStandby = true;

                } else if (gOnIdleMode == ON_IDLE_SHUTDOWN) {
                    if (!gKeepOnWhileCharging || buffer[0].battery != 0)
                        CCRCDCSysConsoleShutdownInd(CCR_CDC_DESTINATION_DRC0);
                    gInShutdown = true;
                }

            }
        }
    }

    if (error) {
        *error = realError;
    }
    return result;
}

DECL_FUNCTION(int32_t, CMPTAcctSetScreenType, CmptScreenType type)
{
    if (gWiiForwarderTVOnly &&
        type == CMPT_SCREEN_TYPE_BOTH &&
        OSGetTitleID() == _SYSGetSystemApplicationTitleId(SYSTEM_APP_ID_HEALTH_AND_SAFETY)) //only do for homebrew
        
        type = CMPT_SCREEN_TYPE_TV;
    return real_CMPTAcctSetScreenType(type);
}

WUPS_MUST_REPLACE_FOR_PROCESS(VPADRead, WUPS_LOADER_LIBRARY_VPAD, VPADRead, WUPS_FP_TARGET_PROCESS_ALL);
WUPS_MUST_REPLACE_FOR_PROCESS(CMPTAcctSetScreenType, WUPS_LOADER_LIBRARY_NN_CMPT, CMPTAcctSetScreenType, WUPS_FP_TARGET_PROCESS_GAME);
