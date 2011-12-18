PROJECT( companion9x )

SET( C9X_VERSION "0.21" )

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
    MESSAGE( FATAL_ERROR "XSD not found ! Please set XSD path ..." )
ENDIF()

ADD_CUSTOM_COMMAND(
  COMMAND ${XSDCXX_EXECUTABLE} cxx-tree --root-element-all --generate-serialization --output-dir ${PROJECT_BINARY_DIR}/ ${PROJECT_SOURCE_DIR}/radio.xsd
  OUTPUT ${PROJECT_BINARY_DIR}/radio.hxx ${PROJECT_BINARY_DIR}/radio.cxx
  DEPENDS ${PROJECT_SOURCE_DIR}/radio.xsd
)

IF( MSVC )

SET(CMAKE_CXX_FLAGS "/EHsc /LD")
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/winbuild C:/Programs/pthreads/pthreads.2)

ELSE( )

ADD_CUSTOM_COMMAND(
  COMMAND make stamp
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../gruvin9x-stable
  OUTPUT ${PROJECT_SOURCE_DIR}/../gruvin9x-stable/stamp-gruvin9x.h
)

ADD_CUSTOM_COMMAND(
  COMMAND make stamp
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../gruvin9x
  OUTPUT ${PROJECT_SOURCE_DIR}/../gruvin9x/stamp-gruvin9x.h
)

ADD_CUSTOM_COMMAND(
  COMMAND make stamp
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../open9x
  OUTPUT ${PROJECT_SOURCE_DIR}/../open9x/stamp-gruvin9x.h
)

ADD_CUSTOM_COMMAND(
  COMMAND patch -f -p0 -r - < ./er9x.patch && touch ${PROJECT_SOURCE_DIR}/../er9x/er9x_patch_applied.h
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
  OUTPUT ${PROJECT_SOURCE_DIR}/../er9x/er9x_patch_applied.h
)

ENDIF()

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
     ${PROJECT_SOURCE_DIR}/../open9x/stamp-gruvin9x.h
     ${PROJECT_SOURCE_DIR}/../gruvin9x/stamp-gruvin9x.h
     ${PROJECT_SOURCE_DIR}/../gruvin9x-stable/stamp-gruvin9x.h
     ${PROJECT_SOURCE_DIR}/../er9x/er9x_patch_applied.h
     # th9xeeprom.cpp
     # th9xinterface.cpp
     er9xeeprom.cpp
     er9xinterface.cpp
     er9xsimulator.cpp
     er9ximport.cpp
     gruvin9xeeprom.cpp
     gruvin9xstableinterface.cpp
     gruvin9xstablesimulator.cpp
     gruvin9xstableimport.cpp
     gruvin9xinterface.cpp  
     gruvin9xsimulator.cpp
     gruvin9ximport.cpp
     gruvin9xv4simulator.cpp
     gruvin9xv4import.cpp  
     open9xeeprom.cpp
     open9xinterface.cpp
     open9xsimulator.cpp  
     open9ximport.cpp
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
     contributorsdialog.cpp
     printdialog.cpp
     fusesdialog.cpp
     expodialog.cpp
     mixerdialog.cpp
     downloaddialog.cpp
     simulatordialog.cpp
     mainwindow.cpp
     main.cpp
)

SET( companion9x_MOC_HDRS
     avroutputdialog.h
     preferencesdialog.h
     burnconfigdialog.h
     printdialog.h
     fusesdialog.h
     mixerdialog.h
     expodialog.h
     contributorsdialog.h
     downloaddialog.h
     simulatordialog.h
     generaledit.h
     modeledit.h
     modelslist.h
     mixerslist.h
     mdichild.h
     mainwindow.h
)
     
SET( companion9x_UIS
     mdichild.ui
     avroutputdialog.ui
     fusesdialog.ui
     expodialog.ui
     mixerdialog.ui
     preferencesdialog.ui
     simulatordialog.ui
     burnconfigdialog.ui
     downloaddialog.ui
     generaledit.ui
     modeledit.ui
     printdialog.ui
     contributorsdialog.ui
)

SET( companion9x_RCS
     companion9x.qrc
     ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/translations.qrc.in ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc @ONLY)

IF( WIN32 )
  SET( companion9x_SRCS ${companion9x_SRCS} icon.rc )
  IF ( NOT MSVC )
    SET(CMAKE_EXE_LINKER_FLAGS -mwindows)
  ENDIF( )
ENDIF( )

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTNETWORK True )
INCLUDE( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )

SET( LANGUAGES he pt ru de fr it )
FOREACH( language ${LANGUAGES} )
  SET( companion9x_TS ${companion9x_TS} companion9x_${language}.ts )
ENDFOREACH( language )
QT4_CREATE_TRANSLATION( QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${companion9x_TS} )

QT4_WRAP_UI( companion9x_SRCS ${companion9x_UIS} )
QT4_WRAP_CPP( companion9x_SRCS ${companion9x_MOC_HDRS} )
QT4_ADD_TRANSLATION( companion9x_QM ${companion9x_TS} )
QT4_ADD_RESOURCES( companion9x_SRCS ${companion9x_RCS} )

# ADD_CUSTOM_TARGET( update_translations DEPENDS ${companion9x_QM} ${companion9x_TS} )

IF( APPLE )
    # Copy the icon files
    # TODO: put these in some sort of add_custom_command( TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ...)
    # also delete the Sigil.app folder as PRE_BUILD of Sigil executable

    #exec_program( "mkdir -p ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/companion9x.app/Contents/Resources")
    #exec_program( "cp ${PROJECT_SOURCE_DIR}/Resource_Files/icon/Sigil.icns ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Sigil.app/Contents/Resources")
    #exec_program( "cp ${PROJECT_SOURCE_DIR}/Resource_Files/icon/sgf.icns ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Sigil.app/Contents/Resources")
    #exec_program( "cp ${PROJECT_SOURCE_DIR}/Resource_Files/icon/epub.icns ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Sigil.app/Contents/Resources")

    # Copy the PLIST file...
    #exec_program( "cp ${PROJECT_SOURCE_DIR}/Resource_Files/mac/MacOSXBundleInfo.plist ${PROJECT_BINARY_DIR}")

    # ...and set the Sigil version string
    #exec_program( "sed -i -e 's/SGVERSION/${SIGIL_FULL_VERSION}/g' ${PROJECT_BINARY_DIR}/MacOSXBundleInfo.plist")

    ADD_EXECUTABLE( ${PROJECT_NAME} MACOSX_BUNDLE ${companion9x_SRCS} ${companion9x_QM} )
    # SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ../mac/MacOSXBundleInfo.plist )
ELSE( )
    ADD_EXECUTABLE( ${PROJECT_NAME} WIN32 ${companion9x_SRCS} ${companion9x_QM} )
ENDIF( )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_LIBRARIES} )

IF( MSVC )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} C:/Programs/pthreads/Pre-built.2/lib/pthread.lib )
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  SET( QTMAIN_LIB qtmaind.lib )
ELSE( )
  SET( QTMAIN_LIB qtmain.lib )
ENDIF( )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${QT_BINARY_DIR}/../lib/${QTMAIN_LIB} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${XERCESC_LIBRARY} )
# LINK_DIRECTORIES( C:/Programs/pthreads/Pre-built.2/lib )
ELSE( )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} xerces-c )
LINK_DIRECTORIES( /usr/local/lib )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} pthread )
ENDIF( )

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION ${QT_BINARY_DIR} )

IF( WIN32 )
  IF( MSVC )
    SET( NSI_IN_FILE companion9x-vs.nsi.in )  
  ELSE( )
    SET( NSI_IN_FILE companion9x-msys.nsi.in )
  ENDIF( )
  
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
  ADD_CUSTOM_TARGET( makedmg
                     COMMAND ${QT_BINARY_DIR}/macdeployqt companion9x.app -dmg -no-plugins
                     # WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                     DEPENDS ${PROJECT_NAME} )
ENDIF( )

ADD_CUSTOM_TARGET( stamp
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/version.h ${CMAKE_SOURCE_DIR}/../companion9x.stamp 
                   DEPENDS ${CMAKE_BINARY_DIR}/version.h )

