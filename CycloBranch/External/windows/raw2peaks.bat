@echo off
SET PATH=%PATH:C:\Xcalibur\system\programs;=%

(type NUL 2>NUL) > "%~1mgf"
for %%f in ("%~d1%~p1%~n1.00*.mgf") do (
echo Converting the file %%f
FileConverter.exe -in "%%f" -out "%%f.FileConverter.mzML" 1>NUL
NoiseFilterGaussian.exe -in "%%f.FileConverter.mzML" -out "%%f.NoiseFilterGaussian.mzML" -algorithm:gaussian_width %2 1>NUL
BaselineFilter.exe -in "%%f.NoiseFilterGaussian.mzML" -out "%%f.BaselineFilter.mzML" 1>NUL
PeakPickerHiRes.exe -in "%%f.BaselineFilter.mzML" -out "%%f.PeakPickerHiRes.mzML" -algorithm:ms_levels 1 2 -algorithm:signal_to_noise 4 1>NUL
FileConverter.exe -in "%%f.PeakPickerHiRes.mzML" -out "%%f.FileConverter2.mgf" 1>NUL
(type "%%f.FileConverter2.mgf" 2>NUL) >> "%~1mgf"
del "%%f" 2>NUL
del "%%f.FileConverter.mzML" 2>NUL
del "%%f.NoiseFilterGaussian.mzML" 2>NUL
del "%%f.BaselineFilter.mzML" 2>NUL
del "%%f.PeakPickerHiRes.mzML" 2>NUL
del "%%f.FileConverter2.mgf" 2>NUL
)
