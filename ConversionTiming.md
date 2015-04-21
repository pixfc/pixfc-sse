## Hardware platforms ##
PixFC-SSE has been tested on Windows, Linux and Mac OSX on the following platforms:
  * [Intel Core i5 760 2.8 GHz](http://pixfc-sse.googlecode.com/svn/trunk/perf-results/i5-details.txt)
  * [Quad core Xeon W3520 2.67 GHz](http://pixfc-sse.googlecode.com/svn/trunk/perf-results/xeon-details.txt)

Click on the above links for more information on the CPU and compiler on each platform.

## Conversion timings ##
The current list of supported conversions and timing information for each OS / platform can be found here:

| OS | CPU | link |
|:---|:----|:-----|
| Linux | i5 | http://pixfc-sse.googlecode.com/svn/trunk/perf-results/i5-linux.txt |
| Linux | Xeon | http://pixfc-sse.googlecode.com/svn/trunk/perf-results/xeon-linux.txt |
| OSX | Xeon |  http://pixfc-sse.googlecode.com/svn/trunk/perf-results/xeon-osx.txt |
| Windows  | i5 | http://pixfc-sse.googlecode.com/svn/trunk/perf-results/i5-win7.txt |

The above pages show the average conversion time in milliseconds, obtained by running each conversion 100 times with a different 1920x1080 input buffer each time. The average time spent in user-space, kernel space and the total number of context switches are also mentioned.