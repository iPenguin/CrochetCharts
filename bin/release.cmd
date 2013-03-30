@echo off
echo +================================
echo   To send the signed installer to the signing host
echo   use the -s switch on the command line.
echo +================================

set signHost=""

IF "%1"=="-s" set signHost="/t http://tsa.starfieldtech.com"

echo "Sign Host: " %signHost%
set appName="Crochet Charts-"
set appExt="-win32.exe"

set /p curVersion="Enter Current Version string (ie 1.1.4): " %=%
set /p csPassword="Enter Code Signing Password: " %=%

cd "C:\Documents and Settings\Brian Milco\My Documents\build"
cmake -G "MinGW Makefiles" ..\crochet.git -DDOCS=ON -DCMAKE_BUILD_TYPE=Release -DQT_QMAKE_EXECUTABLE="C:\Qt\4.8.4\bin\qmake.exe"
cpack -G NSIS

set appFullName=%appName%%curVersion%%appExt%

signtool sign /f "C:\Documents and Settings\Brian Milco\My Documents\Stitch-Works-Software-SWS-Code-Signing.p12" /P %csPassword% %signHost% %appFullName%

cd ..\crochet.git\bin