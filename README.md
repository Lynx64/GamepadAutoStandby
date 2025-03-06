# GamePad Auto Standby
This is a Wii U plugin which makes the GamePad automatically enter standby mode (screen off) or power off when left idle for a configurable amount of time (similar to any other controller).

Additional features:
- Option to power off the GamePad on boot.
  - It will not power off if it was used to turn on the console.
- Option to keep the GamePad on while it's charging/plugged in.
- Power off/on the GamePad remotely from the plugin config menu (obviously it must be in range).

## Installation
For convenience, you can download GamePad Auto Standby directly on your Wii U from the [Homebrew App Store](https://github.com/fortheusers/hb-appstore).

<p align="center">
  <a href="https://hb-app.store/wiiu/GamePadAutoStandby">
    <img width="335" alt="Get it on the Homebrew App Store" src="https://github.com/user-attachments/assets/4471a846-9e8f-4a93-9a5c-a252e70d053a" />
  </a>
</p>

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

## Building
For building you need:
- [wups](https://github.com/wiiu-env/WiiUPluginSystem)
- [wut](https://github.com/devkitPro/wut)

then run `make`
