PROJECT( companion9x )

SET( C9X_VERSION_MAJOR "1" )
SET( C9X_VERSION_MINOR "13" )
SET( C9X_VERSION ${C9X_VERSION_MAJOR}.${C9X_VERSION_MINOR} )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

MESSAGE( STATUS "Looking for XercesC " )
FIND_PACKAGE(XercesC)
IF ( XERCESC_FOUND )
    INCLUDE_DIRECTORIES( ${XERCESC_INCLUDE_DIR} )
    SET(LidarFormat_LIBRAIRIES ${LidarFormat_LIBRAIRIES} ${XERCESC_LIBRARIES})
ELSE()
    MESSAGE( FATAL_ERROR "XercesC not found! Please set XercesC path ..." )
ENDIF()

MESSAGE( STATUS "Looking for Xsd " )
FIND_PACKAGE(Xsd)
IF ( XSD_FOUND )
    INCLUDE_DIRECTORIES( ${XSD_INCLUDE_DIR} )
    MESSAGE("XSD Include Path: " ${XSD_INCLUDE_DIR} )
ELSE()
    MESSAGE( FATAL_ERROR "XSD not found! Please set XSD path ..." )
ENDIF()

MESSAGE( STATUS "Looking for SDL " )
FIND_PACKAGE(Sdl)
IF ( SDL_FOUND )
    INCLUDE_DIRECTORIES( ${SDL_INCLUDE_DIR} )
    MESSAGE("SDL Include Path: " ${SDL_INCLUDE_DIR} )
    ADD_DEFINITIONS( -DJOYSTICKS )
ELSE()
    MESSAGE( "SDL not found! Joysticks support will be disabled" )
ENDIF()

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTNETWORK True )
SET( QT_USE_QTXML True )
INCLUDE( ${QT_USE_FILE} )

MESSAGE( STATUS "Looking for PHONON " )
FIND_PACKAGE(Phonon)
IF ( PHONON_FOUND )
    INCLUDE_DIRECTORIES( ${PHONO_INCLUDE_DIR} )
    MESSAGE("PHONON Include Path: " ${PHONON_INCLUDE_DIR} )
    ADD_DEFINITIONS( -DPHONON )
ELSE()
    MESSAGE( "PHONON not found!" )
ENDIF()

ADD_CUSTOM_COMMAND(
  COMMAND ${XSDCXX_EXECUTABLE} cxx-tree --root-element-all --generate-serialization --output-dir ${PROJECT_BINARY_DIR}/ ${PROJECT_SOURCE_DIR}/radio.xsd
  OUTPUT ${PROJECT_BINARY_DIR}/radio.hxx ${PROJECT_BINARY_DIR}/radio.cxx
  DEPENDS ${PROJECT_SOURCE_DIR}/radio.xsd
)

IF( MSVC )
  SET(CMAKE_CXX_FLAGS "/EHsc /LD")
  INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/winbuild C:/Programs/pthreads/Pre-built.2/include C:/Programs/msinttypes)
  LINK_DIRECTORIES( C:/Programs/pthreads/Pre-built.2/lib/ )
  SET( PTHREAD_LIBRARY pthreadVC2.lib )
ELSE( )
  SET( PTHREAD_LIBRARY pthread )
  ADD_DEFINITIONS( -Wall )
  LINK_DIRECTORIES( /usr/local/lib )
ENDIF( )

INCLUDE_DIRECTORIES( ${XSD_INCLUDE_DIR} )

FIND_PACKAGE( Subversion )
SET( C9X_WC_REVISION 0 )
IF( Subversion_FOUND )
  Subversion_WC_INFO( ${PROJECT_SOURCE_DIR} C9X )
ENDIF( )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

SET( companion9x_SRCS
     eeprominterface.cpp
     hexinterface.cpp
     flashinterface.cpp
     file.cpp
     xmlinterface.cpp
     ${PROJECT_BINARY_DIR}/radio.cxx
     ${PROJECT_SOURCE_DIR}/../open9x/stamp-open9x.h
     ${PROJECT_SOURCE_DIR}/../th9x/stamp-th9x.h
     ${PROJECT_BINARY_DIR}/er9x/stamp-er9x.h
     th9xeeprom.cpp
     th9xinterface.cpp
     th9xsimulator.cpp
     th9ximport.cpp
     er9xeeprom.cpp
     er9xinterface.cpp
     er9xsimulator.cpp
     er9ximport.cpp
     gruvin9xeeprom.cpp
     gruvin9xinterface.cpp  
     open9xeeprom.cpp
     open9xinterface.cpp
     open9xsimulator.cpp  
     open9ximport.cpp
     open9xM128eeprom.cpp
     open9xM128simulator.cpp
     open9xM128import.cpp
     open9xv4eeprom.cpp
     open9xv4simulator.cpp
     open9xv4import.cpp
     open9xarmeeprom.cpp
     open9xarmsimulator.cpp
     open9xarmimport.cpp
     ersky9xeeprom.cpp
     ersky9xinterface.cpp
     ersky9ximport.cpp
     ersky9xsimulator.cpp
     node.cpp
     edge.cpp
     helpers.cpp
     mdichild.cpp
     generaledit.cpp
     modeledit.cpp 
     modelslist.cpp
     mixerslist.cpp 
     avroutputdialog.cpp
     preferencesdialog.cpp
     burnconfigdialog.cpp
     comparedialog.cpp
     contributorsdialog.cpp
     customizesplashdialog.cpp
     burndialog.cpp
     printdialog.cpp
     fusesdialog.cpp
     expodialog.cpp
     mixerdialog.cpp
     logsdialog.cpp
     downloaddialog.cpp
     simulatordialog.cpp
     splashlibrary.cpp
     mainwindow.cpp
     main.cpp
     modelconfigdialog.cpp
     qcustomplot.cpp
)

SET( companion9x_MOC_HDRS
     avroutputdialog.h
     preferencesdialog.h
     burnconfigdialog.h
     comparedialog.h
     printdialog.h
     fusesdialog.h
     mixerdialog.h
     logsdialog.h
     expodialog.h
     contributorsdialog.h
     customizesplashdialog.h
     splashlibrary.h
     splashlabel.h
     burndialog.h
     downloaddialog.h
     cursorwidget.h
     menuwidget.h
     simulatordialog.h
     generaledit.h
     modeledit.h
     modelslist.h
     mixerslist.h
     mdichild.h
     mainwindow.h
     myslider.h
     modelconfigdialog.h
     qcustomplot.h
)
     
SET( companion9x_UIS
     mdichild.ui
     avroutputdialog.ui
     comparedialog.ui
     fusesdialog.ui
     expodialog.ui
     logsdialog.ui
     mixerdialog.ui
     preferencesdialog.ui
     simulatordialog.ui
     burnconfigdialog.ui
     downloaddialog.ui
     generaledit.ui
     modeledit.ui
     printdialog.ui
     contributorsdialog.ui
     customizesplashdialog.ui
     splashlibrary.ui
     burndialog.ui
     modelconfigdialog.ui
)

IF ( SDL_FOUND )
  SET( companion9x_SRCS
       ${companion9x_SRCS}
       joystick.cpp
       joystickdialog.cpp
     )
  
  SET( companion9x_MOC_HDRS
       ${companion9x_MOC_HDRS}
       joystick.h
       joystickdialog.h
     )
     
  SET( companion9x_UIS
       ${companion9x_UIS}
       joystickdialog.ui
     ) 
ENDIF( ) 

SET( companion9x_RCS
     companion9x.qrc
     ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
)

MACRO (TODAY RESULT)
    IF (WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\1.\\2.\\3" ${RESULT} ${${RESULT}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\1.\\2.\\3" ${RESULT} ${${RESULT}})
    ELSE (WIN32)
        MESSAGE(SEND_ERROR "date not implemented")
        SET(${RESULT} 00.00.0000)
    ENDIF (WIN32)
ENDMACRO (TODAY)

MACRO (NOW RESULT)
    IF (WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C time /T" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..):(..).*" "\\1:\\2:00" ${RESULT} ${${RESULT}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%H:%M:%S" OUTPUT_VARIABLE ${RESULT})
        string(REGEX REPLACE "(..):(..):(..).*" "\\1:\\2:\\3" ${RESULT} ${${RESULT}})
    ELSE (WIN32)
        MESSAGE(SEND_ERROR "time not implemented")
        SET(${RESULT} 00:00:00)
    ENDIF (WIN32)
ENDMACRO (NOW)

TODAY(DATE)
NOW(TIME)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/translations.qrc.in ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc @ONLY)

SET( FIRMWARES open9x )
FOREACH( FIRMWARE ${FIRMWARES} )
  Subversion_WC_INFO( ${PROJECT_SOURCE_DIR}/../${FIRMWARE} G9X )
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/stamp-open9x.h.in ${CMAKE_CURRENT_SOURCE_DIR}/../${FIRMWARE}/stamp-open9x.h @ONLY)
ENDFOREACH( FIRMWARE )

SET( PATCH_FLAGS "--binary" )

IF( WIN32 )
  SET( companion9x_SRCS ${companion9x_SRCS} icon.rc )
  IF ( NOT MSVC )
    SET(CMAKE_EXE_LINKER_FLAGS -mwindows)
  ENDIF( )
ENDIF( )

ADD_CUSTOM_COMMAND(
  COMMAND cmake -E remove_directory ${PROJECT_BINARY_DIR}/er9x
  COMMAND cmake -E copy_directory ${PROJECT_SOURCE_DIR}/../er9x ${PROJECT_BINARY_DIR}/er9x
  COMMAND patch ${PATCH_FLAGS} -s -f -p0 < ${PROJECT_SOURCE_DIR}/../er9x.patch
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  OUTPUT ${PROJECT_BINARY_DIR}/er9x/stamp-er9x.h
  DEPENDS ${PROJECT_SOURCE_DIR}/../er9x.patch
)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )


SET( LANGUAGES he pt ru de fr it sv cs )
FOREACH( language ${LANGUAGES} )
  SET( companion9x_TS ${companion9x_TS} companion9x_${language}.ts )
ENDFOREACH( language )

QT4_WRAP_UI( companion9x_SRCS ${companion9x_UIS} )
QT4_WRAP_CPP( companion9x_SRCS ${companion9x_MOC_HDRS} )
QT4_ADD_TRANSLATION( companion9x_QM ${companion9x_TS}  )
QT4_ADD_RESOURCES( companion9x_SRCS ${companion9x_RCS} )

ADD_DEFINITIONS( -DQT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR}" )

IF( APPLE )
    ADD_EXECUTABLE( ${PROJECT_NAME} MACOSX_BUNDLE ${companion9x_SRCS} ${companion9x_QM} )
ELSE( )
    ADD_EXECUTABLE( ${PROJECT_NAME} WIN32 ${companion9x_SRCS} ${companion9x_QM} )
ENDIF( )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${XERCESC_LIBRARY} ${PTHREAD_LIBRARY} ${SDL_LIBRARY} ${PHONON_LIBS} )

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION ${QT_BINARY_DIR} )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    INSTALL( FILES companion9x.desktop DESTINATION share/applications )
    INSTALL( FILES companion9x.png DESTINATION share/icons )
    # Linux specific code
    SET(OperatingSystem "Linux")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF( WIN32 )
  IF( MSVC )
    SET( NSI_IN_FILE companion9x-vs.nsi.in )  
  ELSE( )
    SET( NSI_IN_FILE companion9x-msys.nsi.in )
  ENDIF( )

  SET( SYSDIR "$ENV{windir}/system32" )  
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../winbuild/${NSI_IN_FILE} ${CMAKE_CURRENT_BINARY_DIR}/companion9x.nsi @ONLY)     

  FIND_PROGRAM(NSIS_EXE makensis.exe PATHS
    "C:/Program Files/NSIS"
    "C:/Programs/NSIS"
  )
  
  ADD_CUSTOM_COMMAND(OUTPUT ${PROJECT_BINARY_DIR}/companion9xInstall.exe
                     COMMAND "${NSIS_EXE}" ARGS companion9x.nsi
                     DEPENDS ${PROJECT_NAME} ${PROJECT_BINARY_DIR}/companion9x.nsi
                     COMMENT "Windows NSIS Installer")

  ADD_CUSTOM_TARGET( installer
    DEPENDS ${PROJECT_BINARY_DIR}/companion9xInstall.exe
    SOURCES ${PROJECT_BINARY_DIR}/companion9x.nsi)
ENDIF( )

IF( APPLE )
    SET(MACOSX_BUNDLE_ICON_FILE iconmac.icns)
  IF( CMAKE_GENERATOR STREQUAL Xcode )
    SET( WORK_DIR ${CMAKE_BINARY_DIR}/Debug )
  ELSE( )
    SET( WORK_DIR ${PROJECT_BINARY_DIR} )
  ENDIF()
  
  ADD_CUSTOM_TARGET( addframeworks ALL 
                     COMMAND mkdir -p companion9x.app/Contents/Frameworks
                     COMMAND mkdir -p companion9x.app/Contents/Resources
		     COMMAND mkdir -p companion9x.app/Contents/translations                     
		     COMMAND rsync ${PROJECT_SOURCE_DIR}/images/${MACOSX_BUNDLE_ICON_FILE} companion9x.app/Contents/Resources/${MACOSX_BUNDLE_ICON_FILE} # Copy the icon files
                     COMMAND rsync ${PROJECT_SOURCE_DIR}/../macbuild/lib* companion9x.app/Contents/Frameworks
                     #COMMAND rsync ${PROJECT_SOURCE_DIR}/../macbuild/libcrypto.1.0.0.dylib companion9x.app/Contents/Frameworks
                     COMMAND rsync -r --exclude=.svn  ${PROJECT_SOURCE_DIR}/../macbuild/qt_menu.nib companion9x.app/Contents/Resources/
                     COMMAND rsync ${PROJECT_BINARY_DIR}/*.qm companion9x.app/Contents/translations
		     COMMAND ${QT_BINARY_DIR}/macdeployqt companion9x.app
                     WORKING_DIRECTORY ${WORK_DIR}
                     DEPENDS ${PROJECT_NAME} )

                        
  ADD_CUSTOM_TARGET( makedmg
                     COMMAND ${QT_BINARY_DIR}/macdeployqt companion9x.app -dmg -no-plugins
                     WORKING_DIRECTORY ${WORK_DIR}
                     DEPENDS addframeworks )

ENDIF( )

ADD_CUSTOM_TARGET( stamp
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/version.h ${CMAKE_SOURCE_DIR}/../companion9x.stamp 
                   DEPENDS ${CMAKE_BINARY_DIR}/version.h )

ADD_CUSTOM_TARGET( translations
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                   COMMAND lupdate-qt4 ${CMAKE_SOURCE_DIR} -ts ${companion9x_TS} )

SET(CPACK_PACKAGE_NAME "COMPANION9X")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "EEPROM Editor for open9x / er9x / th9x open source 9X firmwares")
string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE)
find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
if(DPKG_PROGRAM)
  SET(CPACK_GENERATOR "DEB")
  execute_process(
    COMMAND ${DPKG_PROGRAM} --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${C9X_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Romolo Manfredini")
  SET(CPACK_DEBIAN_PACKAGE_VERSION ${C9X_VERSION})
  SET(CPACK_DEBIAN_PACKAGE_VERSION_MAJOR ${C9X_VERSION_MAJOR})
  SET(CPACK_DEBIAN_PACKAGE_VERSION_MINOR ${C9X_VERSION_MINOR})
  SET(CPACK_DEBIAN_ARCHITECTURE $(CMAKE_SYSTEM_PROCESSOR))
  SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
else(DPKG_PROGRAM)
  SET(CPACK_GENERATOR "RPM")
  SET(CPACK_RPM_PACKAGE_VERSION ${C9X_VERSION})
  SET(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}-${C9X_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
endif(DPKG_PROGRAM) 

INCLUDE(CPack)
