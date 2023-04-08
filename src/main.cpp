#include "main.h"
#include "config.h"
#include <wups.h>

// Mandatory plugin info
WUPS_PLUGIN_NAME("GamePad Auto Standby");
WUPS_PLUGIN_DESCRIPTION(DESCRIPTION);
WUPS_PLUGIN_VERSION(VERSION);
WUPS_PLUGIN_AUTHOR("Lynx64");
WUPS_PLUGIN_LICENSE("GPL");

// FS Access
WUPS_USE_WUT_DEVOPTAB();

// Called when exiting the plugin loader
INITIALIZE_PLUGIN()
{
    initConfig();
}
