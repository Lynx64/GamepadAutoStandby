#include "main.h"
#include "config.h"
#include "retain_vars.hpp"
#include "logger.h"
#include <wups.h>
#include <nsysccr/cdc.h>
#include <coreinit/time.h>

// Mandatory plugin info
WUPS_PLUGIN_NAME("GamePad Auto Standby");
WUPS_PLUGIN_DESCRIPTION(DESCRIPTION);
WUPS_PLUGIN_VERSION(VERSION);
WUPS_PLUGIN_AUTHOR("Lynx64");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_USE_WUT_DEVOPTAB();

extern "C" uint32_t OSGetBootPMFlags(void);

// Gets called ONCE when the plugin was loaded
INITIALIZE_PLUGIN()
{
    initConfig();
    
    if (gShutdownOnBoot) {
        uint32_t bootFlags = OSGetBootPMFlags();
        /*  Check that the GamePad wasn't used to turn on the console and
            this isn't an OS relaunch (OSForceFullRelaunch()).
            (Starting at 0) Bit 1 or 27 - GamePad used to turn on the console.
            Bit 13 - OS relaunch (OSForceFullRelaunch()).
            See more https://wiiubrew.org/wiki/Boot1#PowerFlags */
        if ((bootFlags & 0x8002002) == 0) {
            // Determine whether there has been a plugin reload by checking OS uptime
            // Get uptime in ticks and convert to seconds
            OSTime uptime = OSTicksToSeconds(OSGetSystemTime());
            if (uptime < 300) { //super lenient
                //then shutdown GamePad
                DEBUG_FUNCTION_LINE_INFO("Shutting down GamePad on boot (BootPMFlags: 0x%08X , Uptime: %lld secs)", bootFlags, uptime);
                CCRCDCSysConsoleShutdownInd(CCR_CDC_DESTINATION_DRC0);
                gInShutdown = true;
            }
        }
    }
}
