@echo off
mkdir build
copy .\lib\*.dll .\build\
pushd build
cl /I \INCLUDE /I..\inc\ /Zi ..\src\main.c ..\src\game.c Shell32.lib ..\lib\raylib.lib /link /subsystem:CONSOLE
popd
