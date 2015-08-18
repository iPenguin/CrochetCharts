TARGET = CrochetCharts

CONFIG += qt release
QT += core widgets gui xml network svg

DEFINES += gGIT_VERSION='"\\\"$(shell git describe --always)\\\""'
DEFINES += gGIT_VERSION_SHORT='"\\\"$(shell git describe --abbrev=0 --always)\\\""'
DEFINES += gPROJECT_LIFE="2010-2015"

header_files.files = $$HEADERS
header_files.path = $$DESTDIR/
INSTALLS += header_files

CONFIG(release, debug|release) {
	DESTDIR = ../qbuild/release
}
CONFIG(debug, debug|release) {
	DESTDIR = ../qbuild/debug
}

OBJECTS_DIR = $$DESTDIR/
MOC_DIR = $$DESTDIR/
RCC_DIR = $$DESTDIR/
UI_DIR = $$DESTDIR/

INCLUDEPATH += ../src/

RESOURCES += ../crochet.qrc

HEADERS += ../src/aligndock.h
HEADERS += ../src/appinfo.h
HEADERS += ../src/application.h
HEADERS += ../src/cell.h
HEADERS += ../src/chartLayer.h
HEADERS += ../src/chartview.h
HEADERS += ../src/colorlabel.h
HEADERS += ../src/colorlistwidget.h
HEADERS += ../src/colorreplacer.h
HEADERS += ../src/crochetchartcommands.h
HEADERS += ../src/crochettab.h
HEADERS += ../src/debug.h
HEADERS += ../src/errorhandler.h
HEADERS += ../src/exportui.h
HEADERS += ../src/file.h
HEADERS += ../src/file_v1.h
HEADERS += ../src/file_v2.h
HEADERS += ../src/filefactory.h
HEADERS += ../src/guideline.h
HEADERS += ../src/indicator.h
HEADERS += ../src/indicatorundo.h
HEADERS += ../src/itemgroup.h
HEADERS += ../src/legends.h
HEADERS += ../src/mainwindow.h
HEADERS += ../src/mirrordock.h
HEADERS += ../src/propertiesdata.h
HEADERS += ../src/propertiesdock.h
HEADERS += ../src/resizeui.h
HEADERS += ../src/roweditdialog.h
HEADERS += ../src/rowsdock.h
HEADERS += ../src/scene.h
HEADERS += ../src/settings.h
HEADERS += ../src/settingsui.h
HEADERS += ../src/splashscreen.h
HEADERS += ../src/stitch.h
HEADERS += ../src/stitchiconui.h
HEADERS += ../src/stitchlibrary.h
HEADERS += ../src/stitchlibrarydelegate.h
HEADERS += ../src/stitchlibraryui.h
HEADERS += ../src/stitchpalettedelegate.h
HEADERS += ../src/stitchreplacerui.h
HEADERS += ../src/stitchset.h
HEADERS += ../src/tabinterface.h
HEADERS += ../src/textview.h
HEADERS += ../src/undogroup.h
HEADERS += ../src/updatefunctions.h
HEADERS += ../src/updater.h
HEADERS += ../src/version.h

SOURCES += ../src/aligndock.cpp
SOURCES += ../src/appinfo.cpp
SOURCES += ../src/application.cpp
SOURCES += ../src/cell.cpp
SOURCES += ../src/chartLayer.cpp
SOURCES += ../src/chartview.cpp
SOURCES += ../src/colorlabel.cpp
SOURCES += ../src/colorlistwidget.cpp
SOURCES += ../src/colorreplacer.cpp
SOURCES += ../src/crochetchartcommands.cpp
SOURCES += ../src/crochettab.cpp
SOURCES += ../src/debug.cpp
SOURCES += ../src/exportui.cpp
SOURCES += ../src/file.cpp
SOURCES += ../src/file_v1.cpp
SOURCES += ../src/file_v2.cpp
SOURCES += ../src/filefactory.cpp
SOURCES += ../src/guideline.cpp
SOURCES += ../src/indicator.cpp
SOURCES += ../src/indicatorundo.cpp
SOURCES += ../src/itemgroup.cpp
SOURCES += ../src/legends.cpp
SOURCES += ../src/main.cpp
SOURCES += ../src/mainwindow.cpp
SOURCES += ../src/mirrordock.cpp
SOURCES += ../src/propertiesdata.cpp
SOURCES += ../src/propertiesdock.cpp
SOURCES += ../src/resizeui.cpp
SOURCES += ../src/roweditdialog.cpp
SOURCES += ../src/rowsdock.cpp
SOURCES += ../src/scene.cpp
SOURCES += ../src/settings.cpp
SOURCES += ../src/settingsui.cpp
SOURCES += ../src/splashscreen.cpp
SOURCES += ../src/stitch.cpp
SOURCES += ../src/stitchiconui.cpp
SOURCES += ../src/stitchlibrary.cpp
SOURCES += ../src/stitchlibrarydelegate.cpp
SOURCES += ../src/stitchlibraryui.cpp
SOURCES += ../src/stitchpalettedelegate.cpp
SOURCES += ../src/stitchreplacerui.cpp
SOURCES += ../src/stitchset.cpp
SOURCES += ../src/textview.cpp
SOURCES += ../src/undogroup.cpp
SOURCES += ../src/updater.cpp

FORMS += ../src/aligndock.ui
FORMS += ../src/colorreplacer.ui
FORMS += ../src/export.ui
FORMS += ../src/mainwindow.ui
FORMS += ../src/mirrordock.ui
FORMS += ../src/optionsbar.ui
FORMS += ../src/propertiesdock.ui
FORMS += ../src/resize.ui
FORMS += ../src/roweditdialog.ui
FORMS += ../src/rowsdock.ui
FORMS += ../src/settings.ui
FORMS += ../src/stitchicon.ui
FORMS += ../src/stitchlibrary.ui
FORMS += ../src/stitchreplacerui.ui
