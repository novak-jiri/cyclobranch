@echo off
SET PATH=%PATH:C:\Xcalibur\system\programs;=%

FileConverter.exe -in "%~f1.mgf" -out "%~f1.FileConverter.mzML" 1>NUL
rem NoiseFilterGaussian.exe -in "%~f1.FileConverter.mzML" -out "%~f1.NoiseFilterGaussian.mzML" -algorithm:gaussian_width %2 1>NUL
del "%~f1.mzML" 2>NUL
BaselineFilter.exe -in "%~f1.FileConverter.mzML" -out "%~f1.mzML" 1>NUL

del "%~f1.mgf" 2>NUL
del "%~f1.FileConverter.mzML" 2>NUL
rem del "%~f1.NoiseFilterGaussian.mzML" 2>NUL
