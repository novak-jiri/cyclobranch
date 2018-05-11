/usr/local/opt/qt/bin/macdeployqt CycloBranch.app
python macdeployqtfix.py /absolute/path/to/bundle/Contents/MacOS/CycloBranch /usr/local/opt/qt/

mkdir release
mkdir release/CycloBranch
put all folders and app file into the directory CycloBranch
hdiutil create -volname CycloBranch-1.x.xxx -srcfolder release -ov -format UDZO CycloBranch_v1.x.xxx_osx_64bit.dmg
