SET PATH=%PATH%
msconvert.exe %1 --mzML --inten64 --filter "peakPicking vendor msLevel=1-" -o "%~d1%~p1\" --outfile "%~n1_converted.mzML"