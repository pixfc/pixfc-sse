@echo off
call cleanup.bat
cmake -G "Visual Studio 10 Win64" .
pause
