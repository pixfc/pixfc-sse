@echo off
del *.user *.vcproj *.ncb *.sln *.ppm > nul 2>nul
del /A:H *.suo > nul 2>nul
del CMakeCache.txt cmake_install.cmake convert-to-file unit-testing visual-check> nul 2>nul
rd /S /Q CMakeFiles convert-to-file.dir Debug Release RelWithDebInfo pixfc-sse.dir unit-testing.dir visual-check.dir ZERO_CHECK.dir cleanup.dir> nul 2> nul
