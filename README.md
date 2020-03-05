# flutter-arm

A light-weight Flutter Engine Embedder for ARM. It runs without X11.

_The difference between extensions and plugins is that extensions don't include any native code, they are just pure dart. Plugins (like the [connectivity plugin](https://github.com/flutter/plugins/tree/master/packages/connectivity)) include platform-specific code._

## Running your App

### Patching the App

First, you need to override the default target platform in your flutter app, i.e. add the following line to your `main` method, before the `runApp` call:

```dart
debugDefaultTargetPlatformOverride = TargetPlatform.fuchsia;
```

The `debugDefaultTargetPlatformOverride` property is in the foundation library, so you need to import that.

Your main dart file should probably look similiar to this now:

```dart
import 'package:flutter/foundation.dart';

. . .

void main() {
  debugDefaultTargetPlatformOverride = TargetPlatform.fuchsia;
  runApp(MyApp());
}

. . .
```

### Building the Asset bundle

Then to build the asset bundle, run the following commands. You **need** to use a flutter SDK that's compatible to the engine version you're using.

I'm using **flutter_gallery** in this example. (note that the **flutter_gallery** example **does not work** with flutter-arm, since it includes plugins that have no platform-side implementation for ARM yet)

```bash
cd flutter/examples/flutter_gallery
flutter build bundle
```

After that **flutter/examples/flutter_gallery/build/flutter_assets** would be a valid path to pass as an argument to flutter-arm.

### Running your App with flutter-arm

```txt
USAGE:
  flutter-arm [options] <asset bundle path> [flutter engine options...]

OPTIONS:
  -i <glob pattern>   Appends all files matching this glob pattern
                      to the list of input (touchscreen, mouse, touchpad)
                      devices. Brace and tilde expansion is enabled.
                      Every file that matches this pattern, but is not
                      a valid touchscreen / -pad or mouse is silently
                      ignored.
                        If no -i options are given, all files matching
                      "/dev/input/event*" will be used as inputs.
                      This should be what you want in most cases.
                        Note that you need to properly escape each glob pattern
                      you use as a parameter so it isn't implicitly expanded
                      by your shell.

  -h                  Show this help and exit.

EXAMPLES:
  flutter-arm -i "/dev/input/event{0,1}" -i "/dev/input/event{2,3}" /home/helloworld_flutterassets
  flutter-arm -i "/dev/input/mouse*" /home/pi/helloworld_flutterassets
  flutter-arm /home/pi/helloworld_flutterassets
```

**\<asset bundle path\>** is the path of the flutter asset bundle directory (i.e. the directory containing **kernel_blob.bin**)
of the flutter app you're trying to run.

`[flutter engine options...]` will be passed as commandline arguments to the flutter engine. You can find a list of commandline options for the flutter engine [Here](https://github.com/flutter/engine/blob/master/shell/common/switches.h).

## Dependencies

### flutter engine

flutter-arm needs **libflutter_engine.so** and **flutter_embedder.h** to compile. It also needs the flutter engine's **icudtl.dat** at runtime.
You have to options here:

- you build the engine yourself. takes a lot of time, and it most probably won't work on the first try. But once you have it set up, you have unlimited freedom on which engine version you want to use. You can find some rough guidelines [here](https://medium.com/flutter/flutter-on-raspberry-pi-mostly-from-scratch-2824c5e7dcb1). [Andrew jones](https://github.com/andyjjones28) is working on some more detailed instructions.
- you can use the pre-built engine binaries I am providing [in the _engine-binaries_ branch of this project.](https://github.com/ardera/flutter-arm/tree/engine-binaries). I will only provide binaries for some engine versions though (most likely the stable ones).

### graphics libs

Additionally, flutter-arm depends on mesa's OpenGL, OpenGL ES, EGL implementation and libdrm & libgbm.
You can easily install those with

```shell
sudo apt install libgl1-mesa-dev libgles2-mesa-dev libegl-mesa0 libdrm-dev libgbm-dev pkg-config gpiod libgpiod-dev
```

### fonts

The flutter engine, by default, uses the _Arial_ font.

```bash
sudo apt install ttf-mscorefonts-installer fontconfig
sudo fc-cache
```

## Compiling flutter-arm

fetch all the dependencies, clone this repo and run

```bash
cd /path/to/the/cloned/flutter-arm/directory
make
```

The `flutter-arm` executable will then be located at this path: **/path/to/the/cloned/flutter-arm/directory/out/flutter-arm**

## Performance

Performance is actually better than I expected. With most of the apps inside the **flutter SDK -> examples -> catalog** directory I get smooth 50-60fps.

## Keyboard Input

Keyboard input is supported. **There is one important limitation though**. Text input (i.e. writing any kind of text/symbols to flutter input fields) only works when typing on the keyboard, which is attached to the terminal flutter-arm is running on. So, if you ssh into your ARM board to run flutter-arm, you have to enter text into your ssh terminal.

Raw Keyboard input (i.e. using tab to iterate through focus nodes) works with any keyboard attached to your ARM board.

converting raw key-codes to text symbols is not that easy (because of all the different keyboard layouts), so for text input flutter-arm basically uses `stdin`.

## Touchscreen Latency

Due to the way the touchscreen driver works, there's some delta between an actual touch of the touchscreen and a touch event arriving at userspace. The touchscreen driver in the kernel actually just repeatedly polls some buffer shared with the firmware running on the VideoCore, and the videocore repeatedly polls the touchscreen. (both at 60Hz) So on average, there's a delay of 17ms (minimum 0ms, maximum 34ms). If I have enough time in the future, I'll try to build a better touchscreen driver to lower the delay.
