**PixFC-SSE is a cross-platform C library providing SSE-optimised functions to convert between various RGB and YUV formats.**

You specify the input & output formats, PixFC-SSE selects the best conversion function to use based on the features supported by your CPU. In the worst case scenario where the CPU does not support any SSE instructions, PixFC-SSE falls back to a standard, non-SSE conversion function. This way, PixFC-SSE guarantees the conversion will happen regardless of the platform. PixFC-SSE works on Linux, Mac and Windows.

![http://pixfc-sse.googlecode.com/svn/www/lmw-small.png](http://pixfc-sse.googlecode.com/svn/www/lmw-small.png)

