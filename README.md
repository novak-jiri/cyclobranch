# CycloBranch

A tool for mass spectrometry data analysis.

## About
  * designed for identification of small molecules
  * open-source, cross-platform, and stand-alone
  * supported types of spectra
    * conventional mass spectra (MS)
    * liquid-chromatography mass spectra (LC-MS)
    * imaging mass spectra (MSI)
    * tandem mass spectra (MS/MS)
  * main features
    * MS, LC-MC, MSI
      * dereplication / database search
      * prediction of elemental compositions of unknown compounds
      * fine isotopic structure
    * MSI
      * pixel-by-pixel analysis of imaging mass spectra (imzML)
      * fusion of imzML files with optical and histology images
    * MS/MS
      * single spectra only
      * analysis based on custom databases of building blocks (amino acids)
      * linear, cyclic, branched, and branch-cyclic non-ribosomal peptides (NRPs) and siderophores
      * direct comparison of experimental spectrum with theoretical spectrum
      * database search
      * *de novo* sequencing
        * 19 proteinogenic amino acids (20 including isomers)
        * 287 NRP blocks (512 including isomers)

## Supported file formats
  * mzML, imzML
    * profile or centroid spectra
    * profile spectra require [OpenMS 2.x](https://sourceforge.net/projects/open-ms/files/OpenMS/OpenMS-2.4/) installed 
  * mzXML
    * centroid spectra
    * requires [OpenMS 2.x](https://sourceforge.net/projects/open-ms/files/OpenMS/OpenMS-2.4/) installed
  * baf (Bruker)
    * profile and centroid spectra
    * requires [CompassXport 3.0](http://www.bruker.com/service/support-upgrades/software-downloads/mass-spectrometry.html) installed
    * Windows only
  * raw (Thermo)
    * profile or centroid spectra
    * requires [OpenMS 2.x](https://sourceforge.net/projects/open-ms/files/OpenMS/OpenMS-2.4/) including ProteoWizard installed
    * Windows only
  * raw (Waters)
    * profile and centroid spectra
    * Windows only
  * mgf (Mascot Generic File format)
    * centroid spectra
  * txt
    * centroid spectra
    * tab separated mass-to-charge ratio and intensity on each line; multiple peaklists separated by an empty line
  
## Homepage
https://ms.biomed.cas.cz/cyclobranch/
  
## References and More Information
  * molecular formula annotations
    * Novak J. et al. CycloBranch 2: Molecular Formula Annotations Applied to imzML Data Sets in Bimodal Fusion and LC-MS Data Files. *Anal. Chem.*, 92(10):6844–6849, 2020. DOI: [10.1021/acs.analchem.0c00170](https://doi.org/10.1021/acs.analchem.0c00170) 
  * configuration of the tool for different use cases
    * Pluhacek T. et al. Analysis of Microbial Siderophores by Mass Spectrometry. In Sanjoy K. Bhattacharya (ed.), *Metabolomics: Methods and Protocols*, Methods in Molecular Biology, 1996:131-153, Springer, 2019. DOI: [10.1007/978-1-4939-9488-5_12](https://doi.org/10.1007/978-1-4939-9488-5_12)
  * isotopic fine structure
    * Novak J. et al. CycloBranch: An open tool for fine isotope structures in conventional and product ion mass spectra. *J. Mass Spectrom.* 53(11):1097-1103, 2018. DOI: [10.1002/jms.4285](https://doi.org/10.1002/jms.4285)
  * fusion of imzML files with histology images
    * Luptakova D. et al. Non-invasive and Invasive Diagnoses of Aspergillosis in a Rat Model by Mass Spectrometry. *Sci. Rep.*, 7:16523, 2017. DOI: [10.1038/s41598-017-16648-z](https://doi.org/10.1038/s41598-017-16648-z)
  * dereplication in LC-MS and MSI datasets; *de novo* sequencing of siderophores
    * Novak J. et al. Batch-processing of Imaging or Liquid-chromatography Mass Spectrometry Datasets and *De Novo* Sequencing of Polyketide Siderophores. *BBA - Proteins Proteom.*, 1865(7):768-775, 2017. DOI: [10.1016/j.bbapap.2016.12.003](https://doi.org/10.1016/j.bbapap.2016.12.003)
  * *de novo* peptide sequencing of NRPs
    * Novak J. et al. CycloBranch: *De Novo* Sequencing of Nonribosomal Peptides from Accurate Product Ion Mass Spectra. *J. Am. Soc. Mass Spectrom.*, 26(10):1780-1786, 2015. DOI: [10.1007/s13361-015-1211-1](https://link.springer.com/article/10.1007/s13361-015-1211-1)
