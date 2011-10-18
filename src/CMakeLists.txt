PROJECT( companion9x )

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
  COMMAND ${XSD_EXECUTABLE} cxx-tree --root-element-all --generate-serialization --output-dir ${PROJECT_BINARY_DIR}/ ${PROJECT_SOURCE_DIR}/radio.xsd
  OUTPUT ${PROJECT_BINARY_DIR}/radio.hxx ${PROJECT_BINARY_DIR}/radio.cxx
  DEPENDS ${PROJECT_SOURCE_DIR}/radio.xsd
)

INCLUDE_DIRECTORIES( ${XSD_INCLUDE_DIR} )

IF (0)
SET(C9X_Revision 0)
IF( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.svn" )
  # SET(Subversion_SVN_EXECUTABLE "${svn_EXECUTABLE}")
  FIND_PACKAGE(Subversion)
  #IF(Subversion_FOUND)
    SVN_INFO(. SVN_REVISION)
    SET(Mummy_VERSION_SVN_REVISION "${companion9x_WC_LAST_CHANGED_REV}")
    MESSAGE(STATUS  ${Mummy_VERSION_SVN_REVISION} )
    
    #SET(Mummy_VERSION_SVN_REVISION "${Mummy_WC_LAST_CHANGED_REV}")

    # Now the following variables are available for use:
  #  MESSAGE("Subversion_VERSION_SVN='${Subversion_VERSION_SVN}'")
  #  MESSAGE("Subversion_LAST_CHANGED_LOG='${Subversion_LAST_CHANGED_LOG}'")
  #  MESSAGE("Mummy_WC_INFO='${Mummy_WC_INFO}'")
  #  MESSAGE("Mummy_WC_URL='${Mummy_WC_URL}'")
  #  MESSAGE("Mummy_WC_REVISION='${Mummy_WC_REVISION}'")
  #  MESSAGE("Mummy_WC_LAST_CHANGED_AUTHOR='${Mummy_WC_LAST_CHANGED_AUTHOR}'")
  #  MESSAGE("Mummy_WC_LAST_CHANGED_REV='${Mummy_WC_LAST_CHANGED_REV}'")
  #  MESSAGE("Mummy_WC_LAST_CHANGED_DATE='${Mummy_WC_LAST_CHANGED_DATE}'")
  #ENDIF(Subversion_FOUND)
ENDIF( )

SET(C9X_Author "$Author: david.cole $")
SET(C9X_Date "$Date: 2011-01-27 $")
SET(C9X_Id "$Id: CMakeLists.txt $")

SET(C9X_URL "$URL: git://public.kitware.com/mummy.git $")
ENDIF(0)

SET( companion9x_SRCS
     eeprominterface.cpp
     hexinterface.cpp
     file.cpp
     xmlinterface.cpp
     ${PROJECT_BINARY_DIR}/radio.cxx
     er9xeeprom.cpp
     er9xinterface.cpp
     gruvin9xeeprom.cpp
     gruvin9xinterface.cpp  
     open9xeeprom.cpp
     open9xinterface.cpp  
     node.cpp
     edge.cpp
     helpers.cpp 
     mdichild.cpp
     generaledit.cpp
     modeledit.cpp 
     mixerslist.cpp 
     avroutputdialog.cpp
     preferencesdialog.cpp
     burnconfigdialog.cpp
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
     downloaddialog.h
     simulatordialog.h
     generaledit.h
     modeledit.h
     mixerslist.h
     mdichild.h
     mainwindow.h
)
     
SET( companion9x_UIS
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
)

SET( companion9x_RCS
     companion9x.qrc
     ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
)

SET( companion9x_TS
     companion9x_he.ts
     companion9x_pt.ts
     companion9x_ru.ts
     companion9x_de.ts
     companion9x_fr.ts
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/translations.qrc.in ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc @ONLY)
# CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in version.h @ONLY)

IF( WIN32 )
    SET(companion9x_SRCS ${companion9x_SRCS} icon.rc)
    SET(LINK_FLAGS -Wl,-subsystem,windows)
ENDIF( )

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTNETWORK True )
INCLUDE( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

QT4_WRAP_UI( companion9x_SRCS ${companion9x_UIS} )
QT4_WRAP_CPP( companion9x_SRCS ${companion9x_MOC_HDRS} )
QT4_ADD_TRANSLATION( companion9x_QM ${companion9x_TS} )
QT4_ADD_RESOURCES( companion9x_SRCS ${companion9x_RCS} )

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

TARGET_LINK_LIBRARIES( companion9x ${QT_LIBRARIES} )
TARGET_LINK_LIBRARIES( companion9x xerces-c )

INSTALL( TARGETS companion9x DESTINATION ${QT_BINARY_DIR} )

IF( WIN32 )
  FIND_PROGRAM(NSIS_EXE NSIS/makensis.exe)

  ADD_CUSTOM_COMMAND(OUTPUT ../winbuild/companion9xInstall.exe
                     COMMAND "${NSIS_EXE}" ARGS companion9x.nsi
                     # MAIN_DEPENDENCY companion9x.exe
                     DEPENDS ${EXECUTABLE_NAME} ../winbuild/companion9x.nsi
                     #[IMPLICIT_DEPENDS <lang1> depend1 ...]
                     WORKING_DIRECTORY ../winbuild/
                     COMMENT "Windows NSIS Installer")

  ADD_CUSTOM_TARGET( installer
    DEPENDS ../winbuild/companion9xInstall.exe
    SOURCES ../winbuild/companion9x.nsi)
ENDIF( )

IF( APPLE )
  ADD_CUSTOM_TARGET( makedmg
                     COMMAND ${QT_BINARY_DIR}/macdeployqt companion9x.app -dmg -no-plugins
                     # WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                     DEPENDS ${PROJECT_NAME} )
ENDIF( )
