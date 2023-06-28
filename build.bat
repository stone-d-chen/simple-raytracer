@echo off
cl.exe / Zi SDL2_ray.cc  /link build\SDL2main.lib build\SDL2.lib "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22000.0\um\x64\shell32.lib" /SUBSYSTEM:CONSOLE /OUT:build\ray.exe

start build\ray.exe