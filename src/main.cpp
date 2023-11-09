#include "main.h"
#include "config.h"
#include "retain_vars.hpp"
#include <wups.h>
#include <nsysccr/cdc.h>

// Mandatory plugin info
WUPS_PLUGIN_NAME("GamePad Auto Standby");
WUPS_PLUGIN_DESCRIPTION(DESCRIPTION);
WUPS_PLUGIN_VERSION(VERSION);
WUPS_PLUGIN_AUTHOR("Lynx64");
WUPS_PLUGIN_LICENSE("GPLv3");

WUPS_USE_WUT_DEVOPTAB();

extern "C" uint32_t OSGetBootPMFlags(void);

// Called when exiting the plugin loader
INITIALIZE_PLUGIN()
{
    initConfig();
    
    if (gShutdownOnBoot) {
        uint32_t bootFlags = OSGetBootPMFlags();
        if ((bootFlags & 0x8000002) == 0) { //if gamepad not used to turn on the console
            //then shutdown gamepad
            CCRCDCSysConsoleShutdownInd(CCR_CDC_DESTINATION_DRC0);
            gInShutdown = true;
        }
    }
}
