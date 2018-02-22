# CycloBranch

A tool for mass spectrometry data analysis.

## Main features
  * *De novo* sequencing of nonribosomal peptides.
  * Compound search (dereplication) in:
    * conventional mass spectra (MS)
    * liquid-chromatography mass spectra (LC-MS)
    * imaging mass spectra (MSI)
  * Image fusion
  
## Supported file formats
  * mzML, imzML
    * profile or centroid spectra
    * profile spectra require [OpenMS 2.x](http://www.openms.de) installed 
  * mzXML
    * centroid spectra
    * require [OpenMS 2.x](http://www.openms.de) installed
  * baf (Bruker)
    * profile spectra
    * requires [CompassXport 3.0](http://www.bruker.com/service/support-upgrades/software-downloads/mass-spectrometry.html) installed
  * raw (Waters)
    * centroid spectra
  * mgf (Mascot Generic File format)
    * centroid spectra
  * txt
    * centroid spectra
    * tab separated mass-to-charge ratio and intensity on each line; multiple peaklists separated by an empty line
  
## Homepage
http://ms.biomed.cas.cz/cyclobranch/
  
## Publications
  * *J. Am. Soc. Mass Spectrom.* (2015), 26(10):1780-1786. DOI: [10.1007/s13361-015-1211-1](http://dx.doi.org/10.1007/s13361-015-1211-1)
  * *BBA - Proteins Proteom.* (2017), 1865(7):768-775. DOI: [10.1016/j.bbapap.2016.12.003](http://dx.doi.org/10.1016/j.bbapap.2016.12.003)
