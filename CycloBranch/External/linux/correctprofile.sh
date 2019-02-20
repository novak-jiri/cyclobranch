#!/bin/bash
SAVEIFS=$IFS
IFS=$',' read name fwhm <<< $@
IFS=$SAVEIFS
FileConverter -in "$name"".mgf" -out "$name"".FileConverter.mzML"
#NoiseFilterGaussian -in "$name"".FileConverter.mzML" -out "$name"".NoiseFilterGaussian.mzML" -algorithm:gaussian_width $fwhm
rm "$name"".mzML"
BaselineFilter -in "$name"".FileConverter.mzML" -out "$name"".mzML"
rm "$name"".mgf"
rm "$name"".FileConverter.mzML"
#rm "$name"".NoiseFilterGaussian.mzML"
