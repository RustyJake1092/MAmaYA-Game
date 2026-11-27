# MAmaYA-Game

## Pixel World

A small C++17 and raylib sandbox-game foundation. The world is generated deterministically as the player explores, so terrain can be queried at any horizontal coordinate without storing an entire map. Player edits are kept as tile overrides.

## Features

- Side-scrolling camera and pixel-style, tile-based terrain
- Procedural grass, dirt, and underground stone
- Player movement, jumping, gravity, and tile collision
- Mine blocks with the left mouse button and place dirt or stone with the right
- Select dirt or stone with `1` and `2`

## Build on Lubuntu

Install raylib and the C++ build tools. If your Lubuntu release does not package raylib, build the upstream library:

```sh
sudo apt install build-essential cmake git libasound2-dev libx11-dev libxrandr-dev \
  libxi-dev libgl1-mesa-dev libxcursor-dev libxinerama-dev
git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib
cmake -S /tmp/raylib -B /tmp/raylib/build -DBUILD_EXAMPLES=OFF -DBUILD_SHARED_LIBS=ON
cmake --build /tmp/raylib/build --parallel
sudo cmake --install /tmp/raylib/build
sudo ldconfig
```

Then build and run Pixel World from the project directory:

```sh
cmake -S . -B build
cmake --build build
./build/pixel_world
```

Controls are also shown in the game window. Close the window or press `Esc` to quit.

## Android

The Android project is in `android/`. Install Android Studio with Android SDK Platform 35, Android NDK 28.1.13356709, and CMake 3.22.1, then open the `android/` directory in Android Studio and run the `app` configuration on a connected ARM64 Android device. Enable Developer options and USB debugging on the phone first. The first Gradle sync downloads raylib 6.2 from its official GitHub repository.

The Android build uses landscape orientation. On-screen buttons move and jump; tap a tool button, then tap a nearby tile to mine or place it. The block button switches between dirt and stone. To build from a terminal after installing the Android SDK, NDK, and CMake components:

```sh
cd android
./gradlew assembleDebug
```

The debug APK is written to `android/app/build/outputs/apk/debug/app-debug.apk`. Install it on a connected phone from the project root with `adb install -r android/app/build/outputs/apk/debug/app-debug.apk`, or run the `app` configuration from Android Studio.
