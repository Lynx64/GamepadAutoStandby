FROM ghcr.io/wiiu-env/devkitppc:20240704

COPY --from=ghcr.io/wiiu-env/wiiupluginsystem:20240505 /artifacts $DEVKITPRO

WORKDIR project
