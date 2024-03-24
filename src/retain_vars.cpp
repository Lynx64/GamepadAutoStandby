#include "retain_vars.hpp"
#include "config.h"

int32_t gOnIdleMode = DEFAULT_ON_IDLE_MODE_VALUE;
int32_t gStandbyDelay = DEFAULT_STANDBY_DELAY_VALUE;
bool gWiiForwarderTVOnly = DEFAULT_WII_FORWARDER_TV_ONLY_VALUE;
bool gKeepOnWhileCharging = DEFAULT_KEEP_ON_WHILE_CHARGING_VALUE;
bool gShutdownOnBoot = DEFAULT_SHUTDOWN_ON_BOOT_VALUE;

int64_t gStandbyDelayTicks = 0;
bool gInShutdown = false;
