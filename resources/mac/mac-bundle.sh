#!/bin/sh
#make a .app with the given commands

#$1 is the name of the package to make (x.app, MacOS/x, x.icns, etc)
#$2 is the Mach-O binary
#$3 is the Info.plist (does not have to be called info.plist, will be renamed)
#$4 is the binary.icns file
#$5 $6 ... and on are paths to .frameworks (or .dylib files) that need to be bundled with the app

if [ "x$4" == "x" ]; then
{
    echo "usage:"
    echo "APPNAME bundle-name binary-file plist-file icns-file
{frameworks}"
}
else
{
    BUNDLE=$1
    mkdir ${BUNDLE}.app
    mkdir ${BUNDLE}.app/Contents
    mkdir ${BUNDLE}.app/Contents/MacOS
    mkdir ${BUNDLE}.app/Contents/Resources
    mkdir ${BUNDLE}.app/Contents/Frameworks
    mkdir ${BUNDLE}.app/Contents/PlugIns
    cp $2 ${BUNDLE}.app/Contents/MacOS/
    cp $3 ${BUNDLE}.app/Contents/Info.plist
    cp $4 ${BUNDLE}.app/Contents/Resources/
    #copy frameworks
    while [ $# -ge 5 ]; do
    #[ "x$5" == "x" ]; do
    {
        cp -RP $5 ${BUNDLE}.app/Contents/Frameworks/
        shift
    }
    done
}
fi

#Also, if you want to use more customized plist file, you can copy the
#one from CMake/Modules/MacOSXBundleInfo.plist.in to some directory and
#then set CMAKE_MODULE_PATH to that directory.
