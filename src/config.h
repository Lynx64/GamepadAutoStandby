#pragma once

#define DEFAULT_ON_IDLE_MODE_VALUE ON_IDLE_STANDBY
#define DEFAULT_STANDBY_DELAY_VALUE 5
#define DEFAULT_KEEP_ON_WHILE_CHARGING_VALUE false
#define DEFAULT_SHUTDOWN_ON_BOOT_VALUE false

#define ON_IDLE_MODE_CONFIG_ID "onIdleMode"
#define STANDBY_DELAY_CONFIG_ID "standbyDelay"
#define KEEP_ON_WHILE_CHARGING_CONFIG_ID "keepOnWhileCharging"
#define SHUTDOWN_ON_BOOT_CONFIG_ID "shutdownOnBoot"

void initConfig();
