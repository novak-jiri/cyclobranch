@echo off
set PATH=%PATH%
del "%~f1_converted*.csv" 2>NUL
"%programfiles(x86)%\Bruker Daltonik\CompassXport\CompassXport.exe" -mode 4 -a "%~f1" -o "%~f1_converted" 1>NUL
copy "%~f1_converted*.csv" "%~f1.csv" 1>NUL
del "%~f1_converted*.csv" 2>NUL
