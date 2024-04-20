# GamePad Auto Standby
This is a Wii U plugin which makes the GamePad automatically enter standby mode (screen off) or power off when left idle for a configurable amount of time (similar to any other controller).

Additional features:
- Option to keep the GamePad on while it's charging/plugged in.
- Power off/on the GamePad remotely from the plugin config menu (obviously it must be in range).
- Launch .wuhb vWii forwarders on TV only instead of both screens.

## Installation
Download the latest release from the [Releases page](https://github.com/Lynx64/GamepadAutoStandby/releases)<br/>
Copy the `.wps` file into `wiiu/environments/[ENVIRONMENT]/plugins`<br/>
where [ENVIRONMENT] is the actual environment name (most likely 'aroma')

## Usage
Open the plugin config menu by pressing L, DPAD Down and Minus on the GamePad, Pro Controller or Classic Controller, or B, DPAD Down and Minus on a Wii Remote.

Default settings:
- On idle mode: Standby
- Delay: 5 minutes (lowest possible is 1 minute, highest 60)
- Power off on boot: false
- Keep on while charging: false
- Wii forwarders on TV only: true

## Building
For building you need:
- [wups](https://github.com/wiiu-env/WiiUPluginSystem)
- [wut](https://github.com/devkitPro/wut)

then run `make`
