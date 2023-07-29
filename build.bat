@echo off
@set INCPATH=include
@set SOURCES=SDL2_ray.cc include\imgui_impl_sdl2.cpp include\imgui_impl_sdlrenderer2.cpp include\imgui*.cpp
@set LIBS=build\SDL2main.lib build\SDL2.lib "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22000.0\um\x64\shell32.lib"
cl.exe /O2 /I %INCPATH% %SOURCES% /link %LIBS% /SUBSYSTEM:CONSOLE /OUT:build\ray.exe

start build\ray.exe