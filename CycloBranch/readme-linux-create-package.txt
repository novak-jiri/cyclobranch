cp -r CycloBranch cyclobranch-2.x.xxx
cd cyclobranch-2.x.xxx
find . -type f -exec chmod 664 -- {} +
find . -type d -exec chmod 775 {} +
remove all unneccessary files
mkdir build, mkdir moc, source files and project file must be included !
qmake
dh_make -e jiri.novak@biomed.cas.cz --createorig
edit the files debian/control and debian/cyclobranch.install (the cyclobranch.install file must not be executable !)
chmod 664 debian/control
chmod 664 debian/cyclobranch.install
dpkg-buildpackage -b -uc

mkdir CycloBranch_v2.x.xxx_linux_64bit
add cyclobranch_2.x.xxx-1_amd64.deb and readme-linux-install.txt
chmod 664 readme-linux-install.txt
tar -cvzf CycloBranch_v2.x.xxx_linux_64bit.tgz CycloBranch_v2.x.xxx_linux_64bit

create rpm file using: 
mkdir rpm
mv cyclobranch_2.x.xxx-1_amd64.deb rpm/
cd rpm
sudo alien -r cyclobranch_2.x.xxx-1_amd64.deb

