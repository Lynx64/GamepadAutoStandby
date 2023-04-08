# GamePad Auto Standby
This is a Wii U plugin which makes the GamePad automatically enter standby mode or power off when left idle for a configurable amount of time (similar to any other controller).

## Installation
Download the latest release from [here](https://github.com/Lynx64/GamepadAutoStandby/releases)<br/>
Copy the .wps file into `wiiu/environments/[ENVIRONMENT]/plugins`<br/>
where [ENVIRONMENT] is the actual environment name (most likely 'aroma')

## Usage
Open the plugin config menu by pressing L, DPAD Down and Minus on the GamePad, Pro Controller or Classic Controller, or B, DPAD Down and Minus on a Wii Remote.

## Building
For building you need:
- [wups](https://github.com/wiiu-env/WiiUPluginSystem)
- [wut](https://github.com/devkitPro/wut)

then run `make`
