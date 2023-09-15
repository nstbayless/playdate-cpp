# Playdate-C++

*for SDK version 2.0.3, but attempts will be made to keep this up to date*

You want to use C++ on the playdate! Great, except you keep getting linker
errors about exception handling, and your global variables aren't getting
initialized properly, and `__attribute__((constructor))` isn't helping, and...

## Prerequisites
1. [CMake](https://cmake.org/download/)
2. A compiler capable of handling C++20 (Only a concern for the simulator, the
   `arm-none-eabi` compiler referenced by prerequisites section from
   ["Inside Playdate With C"](https://sdk.play.date/2.0.3/Inside%20Playdate%20with%20C.html)
   supports this.)
3. For the Playdate hardware, you need to have the ARM toolchain of version
   12.3.rel1 or later. If you don't you may encounter linker errors.
4. The Playdate SDK installed (and preferably added to the `PATH`)
5. The forward declaration of `eventHandler` in `pd_api.h` needs to be deleted 
   or commented out. At time of writing, this is lines 58-61, there is also a
   comment indicating that is the main entry point function. This will not 
   impact pure C builds, including the builds of all the Playdate examples. Not
   doing this will lead to linker errors, if you're seeing errors like "linkage
   specification contradicts earlier specification for 'eventHandler'", you did
   not remove this forward declaration.

## How to use
### Project Setup:
Follow along with the `CMakeLists.txt` file in the "Hello World" example:
1. Add this repo as a subdirectory of your project.
2. Use the `add_subdirectory` directive to include this package in your project
3. Use the `add_playdate_application` directive with your source to add your
   application to the build

Follow along with the `main.cpp` file in the "Hello World" example:
1. Surround the `eventHandler` function with and `extern C` statement to ensure
   correct linkage.
2. call `eventHandler_pdnewlib` first thing in your `eventHandler`

### Building
This uses the CMake build system, which is fully cross-platform. Many IDEs like
CLion and VSCode either integrate directly with CMake, or provide extensions
which do so. Otherwise, refer to the documentation from [CMake](https://cmake.org/runningcmake/)
for the recommended process for building on your platform from the command line
or CMake GUI.

#### To build for the playdate hardware
A toolchain file needs to be specified. This can be done in the `CMake options`
field in a build configuration in CLion, or from the command line, with the
following option:
```
-DCMAKE_TOOLCHAIN_FILE=<path_to_playdate_SDK>/C_API/buildsupport/arm.cmake
```

#### To build the examples
Same as the toolchain file, this is achieved with an option passed to CMake:
```
-DPDCPP_BUILD_EXAMPLES=ON
```

## This doesn't work for me
The base principles of this are explained on this [forum thread](https://devforum.play.date/t/cpp-guide-c-on-playdate/5085). 
Good luck.

## This doesn't work at all
Yikes!?‽!? Please submit a bug report to NaOH#1432 or MrBZapp on discord, or if
you're able, a please submit pull request with a fix :) :)
