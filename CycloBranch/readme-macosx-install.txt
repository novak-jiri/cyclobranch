1) open CycloBranch-1.x.xxx.dmg, then drag and drop the CycloBranch folder into your "/Applications" folder 

2) If you would like to use mzML, mzXML and imzML file formats:
- download and install OpenMS 2.0 (or higher; http://www.openms.de)
- put the line "setenv PATH /usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/Applications/OpenMS-2.0/TOPP" into the file "/etc/launchd.conf"
- type "sudo chmod +x /Applications/CycloBranch/External/macosx/any2mgf.sh"
- type "sudo chmod +x /Applications/CycloBranch/External/macosx/correctprofile.sh"
- type "sudo chmod +x /Applications/CycloBranch/External/macosx/raw2peaks.sh"





