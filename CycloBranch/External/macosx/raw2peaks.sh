SAVEIFS=$IFS
IFS=$',' read name fwhm <<< $@
IFS=$SAVEIFS
if [ -f "$name""mgf" ]
then
rm "$name""mgf"
fi
touch "$name""mgf"
for i in "$name""00"*".mgf"
do
echo "Converting the file ""$i"
FileConverter -in "$i" -out "$i"".FileConverter.mzML"
NoiseFilterGaussian -in "$i"".FileConverter.mzML" -out "$i"".NoiseFilterGaussian.mzML" -algorithm:gaussian_width $fwhm
BaselineFilter -in "$i"".NoiseFilterGaussian.mzML" -out "$i"".BaselineFilter.mzML"
PeakPickerHiRes -in "$i"".BaselineFilter.mzML" -out "$i"".PeakPickerHiRes.mzML" -algorithm:ms_levels 1 2 -algorithm:signal_to_noise 4
FileConverter -in "$i"".PeakPickerHiRes.mzML" -out "$i"".FileConverter2.mgf"
cat "$i"".FileConverter2.mgf" >> "$name""mgf"
rm "$i"
rm "$i"".FileConverter.mzML"
rm "$i"".NoiseFilterGaussian.mzML"
rm "$i"".BaselineFilter.mzML"
rm "$i"".PeakPickerHiRes.mzML"
rm "$i"".FileConverter2.mgf"
done
