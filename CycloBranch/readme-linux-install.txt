1) To install CycloBranch, type the following commands:
sudo apt-get install qt5-default
sudo apt-get install qt5-image-formats-plugins
sudo apt-get install libqt5svg5*
sudo apt-get install libboost-all-dev
sudo apt-get install libxerces-c-dev
sudo dpkg -i cyclobranch_1.x.xxx-1_amd64.deb

2) Run CycloBranch using the command:
CycloBranch

3) If you would like to use mzML, mzXML and imzML file formats, download and install OpenMS 2.3.0 (http://www.openms.de) and type the following commands:
sudo sudo dpkg -i OpenMS-2.3.0-Linux.deb
sudo chmod +x /usr/share/cyclobranch/External/linux/any2mgf.sh
sudo chmod +x /usr/share/cyclobranch/External/linux/raw2peaks.sh

4) If you would like to export images in PDF, type "sudo apt-get install cups-pdf" and configure CUPS PDF print server.
PostScript files (PS) must be created outside of the application, e.g., using the pdf2ps tool.

5) If you need a rpm file instead of deb file, use "sudo alien -r cyclobranch_1.x.xxx-1_amd64.deb" to create the rpm file.

6) To remove CycloBranch, type the following command: "sudo dpkg -r cyclobranch".
