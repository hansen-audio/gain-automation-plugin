# Gain Automator Effect Plug-in
[![Linux Build](https://github.com/hansen-audio/gain-automator/actions/workflows/cmake_linux.yml/badge.svg)](https://github.com/hansen-audio/gain-automator/actions/workflows/cmake_linux.yml) [![macOS Build](https://github.com/hansen-audio/gain-automator/actions/workflows/cmake_macos.yml/badge.svg)](https://github.com/hansen-audio/gain-automator/actions/workflows/cmake_macos.yml)

A simple VST 3 plug-in which can automate the gain parameter with sample accuracy. It is a precondition that the host supports VST 3's sample accurate automation curves.

## Building the project

Execute the following commands on cli.

```
git clone /path/to/gain-automator.git
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ../gain-automator
cmake --build .
```

> Linux: ```cmake -DCMAKE_BUILD_TYPE=[Debug|Release] ../gain-automator```
> macOS:```cmake -GXcode ../gain-automator```
> Windows 10: ```cmake -G"Visual Studio 16 2019" -A x64 ..\gain-automator```

## License

Copyright 2021 Hansen Audio

Licensed under the GPLv3: http://www.gnu.org/licenses/gpl-3.0.html

VST 3 and the VST 3 logo are trademarks of Steinberg Media Technologies GmbH.
