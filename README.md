# CycloBranch

An open-source, cross-platform and stand-alone tool for mass spectrometry data analysis.

## Main features
  * designed for identification of small molecules
  
  * *de novo* sequencing
    * non-ribosomal peptides (NRPs) [1]
    * siderophores (including metals) [2]
    * supports linear, cyclic, branched, and branch-cyclic structures
    * based on custom databases of building blocks
      * 19 proteinogenic amino acids (20 including isomers)
      * 287 NRP blocks (512 including isomers)
  
  * dereplication / database search:
    * conventional mass spectra (MS)
    * liquid-chromatography mass spectra (LC-MS) [2]
      * batch-processing
    * imaging mass spectra (MSI)
      * pixel-by-pixel analysis of imaging mass spectra (imzML) [2]
      * fusion of imzML files with optical and histology images [3]
    * tandem mass spectra (MS/MS)
      * single spectra analysis
      * supports also a direct comparison of an experimental spectrum with a theoretical one
    
## Supported file formats
  * mzML, imzML
    * profile or centroid spectra
    * profile spectra require [OpenMS 2.x](https://sourceforge.net/projects/open-ms/files/OpenMS/OpenMS-2.3/) installed 
  * mzXML
    * centroid spectra
    * requires [OpenMS 2.x](https://sourceforge.net/projects/open-ms/files/OpenMS/OpenMS-2.3/) installed
  * baf (Bruker)
    * profile spectra
    * requires [CompassXport 3.0](http://www.bruker.com/service/support-upgrades/software-downloads/mass-spectrometry.html) installed
    * Windows only
  * raw (Thermo)
    * profile or centroid spectra
    * requires [OpenMS 2.x](https://sourceforge.net/projects/open-ms/files/OpenMS/OpenMS-2.3/) including ProteoWizard installed
    * Windows only
  * raw (Waters)
    * centroid spectra
    * Windows only
  * mgf (Mascot Generic File format)
    * centroid spectra
  * txt
    * centroid spectra
    * tab separated mass-to-charge ratio and intensity on each line; multiple peaklists separated by an empty line
  
## Homepage
http://ms.biomed.cas.cz/cyclobranch/
  
## Publications
  * [1] *J. Am. Soc. Mass Spectrom.* (2015), 26(10):1780-1786. DOI: [10.1007/s13361-015-1211-1](http://dx.doi.org/10.1007/s13361-015-1211-1)
  * [2] *BBA - Proteins Proteom.* (2017), 1865(7):768-775. DOI: [10.1016/j.bbapap.2016.12.003](http://dx.doi.org/10.1016/j.bbapap.2016.12.003)
  * [3] *Sci. Rep.* (2017), 7:16523. DOI: [10.1038/s41598-017-16648-z](http://dx.doi.org/10.1038/s41598-017-16648-z)
