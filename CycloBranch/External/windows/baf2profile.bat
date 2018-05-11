@echo off
set PATH=%PATH%
del "%~f1_converted*.csv" 2>NUL
del "%~f1.profile.*.csv" 2>NUL
"%programfiles(x86)%\Bruker Daltonik\CompassXport\CompassXport.exe" -mode 4 -raw 1 -a "%~f1" -o "%~f1_converted" 1>NUL
setlocal enabledelayedexpansion
set count=0
for %%i in ("%~f1_converted_*.csv") do (
set /a count=!count!+1
move "%%i" "%~f1.profile.!count!.csv" 1>NUL
)