**These instructions are ONLY for those who want to compile PixFC manually instead of using the provided binary packages. You do not need to do this if you have followed the instructions on the HowToUse page.**

Building PixFC manually can be a lengthy process, specially if your build machine is not that recent. If you just want to try out PixFC and see how it performs on a machine, I suggest you download a binary package instead and run the included conversion timing tool, as explained on the HowToUse page.

## Working configurations ##
PixFC can be built either on Mac, Windows or Linux. However, you must pay attention to the compiler and the compiler version you use. The table below summarises the configurations known to work for both 32- and 64-bit builds:

| **OS** | **Version** | **Toolchain** | **Version** |
|:-------|:------------|:--------------|:------------|
| Linux | Ubuntu 10.04 | gcc | Ubuntu 4.4.3-4ubuntu5 4.4.3 |
| Linux | Ubuntu 12.04 | gcc | Ubuntu/Linaro 4.6.3-1ubuntu5 4.6.3  |
| OSX | 10.6.6 | gcc | i686-apple-darwin10-gcc-4.2.1 4.2.1 (Apple Inc. build 5664)|
| OSX | 10.7.4 | clang | Apple clang version 3.1 (tags/Apple/clang-318.0.58) (based on LLVM 3.1svn) |
| Windows | 7 home edition | Visual Studio | 2010 |

## Prerequisites ##
Make sure you use one of the above configuration. Then you need to :
  * Install [CMake](http://www.cmake.org)
  * Download the PixFC source code (head to the [Downloads](http://code.google.com/p/pixfc-sse/downloads/list) tab).

Assuming you have unpacked PixFC's source code in `pixfc/`, follow the instructions for your platform / environment. These will help you compile the PixFC library for a specific target platform or for the platform you will compile PixFC on.

## Windows - Visual Studio solution ##

Run a command prompt and go to `pixfc\build\`.

Then run either of the following command depending on whether you want to compile PixFC in 32- or 64-bit mode:
```
cmake -G "Visual Studio 10" ..
```
```
cmake -G "Visual Studio 10 Win64" ..
```

This will generate a Visual Studio solution (`pixfc-sse.sln`). Open it, make sure the build configuration is set to Release and build the solution.
The PixFC library (`pixfc-sse.lib`) can then be found in the `src/Release` directory.

## Mac & Linux - Makefile ##

In a terminal, change to `pixfc/build/` and run the following commands:
```
cmake ..
make
```

This will build PixFC library (`libpixfc-sse.a`) in `src`.
In `pixfc/cmake/`, you will find a couple of CMake toolchain files for cross-compiling PixFC for a variety of platforms using different compilers, including DistCC which can be very useful in speeding up the build process. To use one of the CMake toolchain file, just add`-DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-....cmake` to the list of arguments when running the `cmake` command. No need to supply a toolchain file on Windows ...

## All Platforms - Eclipse CDT 4 ##

From a terminal / command prompt, create a `eclipse-proj/` directory side by side with `pixfc/`. It important that the directory is created at the same level as `pixfc/`. Then change to `eclipse-proj` and run:
```
cmake -G "Eclipse CDT 4 - Unix Makefiles" ../pixfc
```
Then, import the project in Eclipse to build it.


The next step is to include PixFC in you project and use it as described [here](HowToUse.md).