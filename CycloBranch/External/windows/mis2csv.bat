@echo off
set PATH=%PATH%
cd %~f1

del "%~f1.baf.csv" 2>NUL
del "%~f1.baf.txt" 2>NUL

for /R %%f in (analysis.baf) do if exist "%%f" (
del "%%f_converted*.csv" 2>NUL
"%programfiles(x86)%\Bruker Daltonik\CompassXport\CompassXport.exe" -mode 4 -a "%%f" -o "%%f_converted" 1>NUL
(type "%%f_converted*.csv" 2>NUL) >> "%~f1.baf.csv"
del "%%f_converted*.csv" 2>NUL
)

for /d %%i in (*) do (
echo %%i >> "%~f1.baf.txt"
)
