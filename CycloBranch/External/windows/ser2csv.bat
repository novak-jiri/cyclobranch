@echo off
set PATH=%PATH%
cd /d %~f1

del "%~f1.csv" 2>NUL
del "%~f1.txt" 2>NUL

for /R %%f in (fid) do if exist "%%f" (
del "%%f_converted*.csv" 2>NUL
CompassXport.exe -mode 4 -a "%%f" -o "%%f_converted" -raw 1 1>NUL
(type "%%f_converted*.csv" 2>NUL) >> "%~f1.csv"
del "%%f_converted*.csv" 2>NUL
echo %%f >> "%~f1.txt"
)
