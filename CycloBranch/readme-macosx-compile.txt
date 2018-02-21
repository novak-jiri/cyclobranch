Tested on OS X Mountain Lion:

1) Prerequisities:
- download and install Xcode 4.6.2 (release date 15.4.2013)
- download and install Command Line Tools for Xcode 4.6.2 (OS X Mountain Lion)
- download and install Qt 5.2.1 
- download and install brew and type "brew doctor"
- install boost 1.57.0 using "brew install boost"
- install xerces-c 3.1.1 using "brew install xerces-c"
- install gcc using "brew install gcc"

2) Execute the following commands in the directory where CycloBranch is unpacked:
- type "/opt/local/share/Qt5.2.1/5.2.1/clang_64/bin/qmake"
- open "Makefile" using a text editor and correct the following lines:
CC            = /usr/local/Cellar/gcc/4.9.2_1/bin/gcc-4.9
CXX           = /usr/local/Cellar/gcc/4.9.2_1/bin/g++-4.9
LINK          = /usr/local/Cellar/gcc/4.9.2_1/bin/g++-4.9
- type "make"

