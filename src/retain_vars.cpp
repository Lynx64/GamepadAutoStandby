#include "retain_vars.hpp"

OnIdleModeEnum gOnIdleMode = ON_IDLE_STANDBY;
int32_t gStandbyDelay = 5;
int64_t gStandbyDelayTicks = 0;
bool gInShutdown = false;
bool gWiiForwarderTVOnly = true;
bool gKeepOnWhileCharging = false;
