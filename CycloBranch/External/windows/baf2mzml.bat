@echo off
set PATH=%PATH%
"%programfiles(x86)%\Bruker Daltonik\CompassXport\CompassXport.exe" -mode 2 -a "%~f1" -o "%~f1.mzML" 1>NUL
