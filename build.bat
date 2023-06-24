@echo off
cl.exe /O2 ray.cpp  /link libSDL\SDL2main.lib libSDL\SDL2.lib "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22000.0\um\x64\shell32.lib" /SUBSYSTEM:CONSOLE

start ray.exe