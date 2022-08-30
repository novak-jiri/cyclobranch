Tested on macOS Sierra (10.12):

1) Prerequisities:
- download and install Xcode 8.3.2 (or higher) and Command Line Tools for Xcode 8.3.2 (or higher) from https://developer.apple.com/download/more/
- type "sudo xcode-select -s /Applications/Xcode.app/Contents/Developer" (if Xcode is installed in the /Applications folder)
- type "sudo xcodebuild -license" and agree with the license
- download and install brew from https://brew.sh/ and type "brew doctor"
- install qt 5.9 (or higher) using "brew install qt"
- install xerces-c 3.1.4 (or higher) using "brew install xerces-c"
- install boost 1.64.0 (or higher) using "brew install boost"

2) Execute the following commands in the directory where CycloBranch is unpacked:
- open "images.qrc" in TextEdit and click File->Save
- open "CycloBranch-MacOSX.pro" in TextEdit and click File->Save
- type "/usr/local/opt/qt/bin/qmake"
- type "make"

