include_directories(${QT_INCLUDES} ${CMAKE_BINARY_DIR}/src ${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(QT_USE_QTMAIN true)
set(QT_USE_QTTEST true)

set(CMAKE_AUTOMOC true)

include(${QT_USE_FILE})

file(GLOB crochet_test_srcs "test*.cpp")
file(GLOB crochet_test_qobject_headers "test*.h")

file(GLOB crochet_app_h "../src/*.h")
set(crochet_app_cpp  
    ../src/aligndock.cpp    
    ../src/colorlistwidget.cpp       
    ../src/file.cpp         
    ../src/indicatorundo.cpp  
    ../src/propertiesdata.cpp  
    ../src/settingsui.cpp             
    ../src/stitchlibraryui.cpp        
    ../src/updater.cpp
    ../src/appinfo.cpp      
    ../src/colorreplacer.cpp         
    ../src/filefactory.cpp  
    ../src/itemgroup.cpp      
    ../src/propertiesdock.cpp  
    ../src/splashscreen.cpp           
    ../src/stitchpalettedelegate.cpp
    ../src/application.cpp  
    ../src/crochetchartcommands.cpp  
    ../src/file_v1.cpp      
    ../src/legends.cpp        
    ../src/roweditdialog.cpp   
    ../src/stitch.cpp                 
    ../src/stitchreplacerui.cpp
    ../src/cell.cpp         
    ../src/crochettab.cpp            
    ../src/file_v2.cpp
    ../src/rowsdock.cpp        
    ../src/stitchiconui.cpp           
    ../src/stitchset.cpp
    ../src/chartview.cpp    
    ../src/debug.cpp                 
    ../src/guideline.cpp    
    ../src/mainwindow.cpp     
    ../src/scene.cpp           
    ../src/stitchlibrary.cpp          
    ../src/textview.cpp
    ../src/colorlabel.cpp   
    ../src/exportui.cpp              
    ../src/indicator.cpp    
    ../src/mirrordock.cpp     
    ../src/settings.cpp        
    ../src/stitchlibrarydelegate.cpp  
    ../src/undogroup.cpp
    ${CMAKE_BINARY_DIR}/version.cpp )



file(GLOB crochet_ui_h "../src/*.ui")

set(crochet_app_resources
    ../crochet.qrc
    ../stitches/stitches.qrc
    )

qt4_add_resources(crochet_app_rcc_srcs ${crochet_app_resources})
#qt4_wrap_cpp(crochet_test_moc_srcs ${crochet_test_qobject_headers} ${crochet_app_h})
qt4_wrap_ui(crochet_ui_h ${crochet_ui})

add_executable(tests main.cpp ${crochet_test_srcs} ${crochet_test_moc_srcs} ${crochet_app_rcc_srcs} ${crochet_app_cpp} ${crochet_ui_h})
target_link_libraries(tests ${QT_LIBRARIES})
