The fastest way to test PixFC and/or include it in your application is to:
  * [Download the PixFC binaries](HowToUse#Download_PixFC.md)
  * [Include PixFC in your project](HowToUse#Include_PixFC_in_your_project.md)

Instead of downloading the pre-built binaries, you can also download the source code and [build PixFC yourself](BuildingPixFC.md) (much longer).

# Download and test PixFC #

Head to the [Downloads](http://code.google.com/p/pixfc-sse/downloads/list) tab for a list of latest stable packages, and pick **the binary package which matches your platform**. If your OS supports 64-bit, make sure you grab a 64-bit binary package !

After unpacking the archive, you can run the `time_conversions` app to see how fast PixFC performs on you platform. You can compare your results with [these](ConversionTiming.md). Please take some time to submit your results on the [mailing list](http://groups.google.com/group/pixfc-sse), along with details of your platform (OS version, CPU name & model).

# Include PixFC in your project #

Assuming you unpacked the PixFC archive in `pixfc/`, either :
  * add the `pixfc/include` directory to your application's include path (on Windows, also add `pixfc\include\win`), OR
  * copy `pixfc/include/pixfc-sse.h` to one of the directories in your application's include path (On Windows, also copy `pixfc\include\win\stdint.h`).

Next, `#include "pixfc-sse.h"` where appropriate (so you can call PixFC functions). The use-case is then pretty simple:
  * Create a `struct PixFcSSE` by calling `create_pixfc(...)`. You specify the input and output formats, resolution, and optional flags to control the selection of the conversion routine.
  * Then, every time you need to convert an image, call `struct PixFcSSE->convert(...)`. Note that **better performance is achieved when the input and output buffers are 16-byte aligned, but they dont have to be 16-byte aligned**.
  * Release the `struct PixFcSSE` when you no longer need it by calling `destroy_pixfc(...)`.

Take a look at `pixfc/example.c` for a concrete example. The main header file (`pixfc-sse.h`) is also a good source of information.

Last, link the PixFC static library (`pixfc/libpixfc-sse.a` on Mac & Linux, `pixfc/pixfc-sse.lib` on Windows) to your application. On Linux, also link your application against librt by passing `-lrt` to the linker.
That's it !.