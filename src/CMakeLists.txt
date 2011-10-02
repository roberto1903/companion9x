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
     donatorsdialog.cpp
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
     donatorsdialog.h
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
     donatorsdialog.ui
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
)

SET( companion9x_TS
     companion9x_he.ts
     companion9x_pt.ts
     companion9x_ru.ts
     companion9x_de.ts
     companion9x_fr.ts
)

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTNETWORK True )
INCLUDE( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

QT4_WRAP_UI( companion9x_SRCS ${companion9x_UIS} )
QT4_WRAP_CPP( companion9x_SRCS ${companion9x_MOC_HDRS} )
QT4_ADD_TRANSLATION( companion9x_QM ${companion9x_TS} )
QT4_ADD_RESOURCES( companion9x_SRCS ${companion9x_RCS} )

ADD_EXECUTABLE( companion9x ${companion9x_SRCS} ${companion9x_QM} )
TARGET_LINK_LIBRARIES( companion9x ${QT_LIBRARIES} )
TARGET_LINK_LIBRARIES( companion9x xerces-c )
 
INSTALL( TARGETS companion9x DESTINATION ${QT_BINARY_DIR} )
