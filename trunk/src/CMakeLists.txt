PROJECT( companion9x )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

MESSAGE( STATUS "Looking for xsdcxx " )
FIND_PROGRAM( xsdcxx_BIN NAMES xsdcxx xsd xsd.exe )
GET_FILENAME_COMPONENT(xsdcxx_DIR ${xsdcxx_BIN} PATH )
FIND_PATH( xsdcxx_INC xsd/cxx/config.hxx /usr/local/include /usr/include "${xsdcxx_DIR}/../libxsd" )
IF( NOT "${xsdcxx_BIN}" STREQUAL "xsdcxx_BIN-NOTFOUND" AND NOT "${xsdcxx_INC}" STREQUAL "xsdcxx_INC-NOTFOUND" )
  MESSAGE( STATUS "  found xsdcxx: ${xsdcxx_BIN} ${xsdcxx_INC}")
ELSE( )
  MESSAGE( STATUS "  xsdcxx not found" )
ENDIF( )

ADD_CUSTOM_COMMAND(
  COMMAND ${xsdcxx_BIN} cxx-tree --root-element-all --generate-serialization --output-dir ${PROJECT_BINARY_DIR}/ ${PROJECT_SOURCE_DIR}/radio.xsd
  OUTPUT ${PROJECT_BINARY_DIR}/radio.hxx ${PROJECT_BINARY_DIR}/radio.cxx
  DEPENDS ${PROJECT_SOURCE_DIR}/radio.xsd
)

INCLUDE_DIRECTORIES( ${xsdcxx_INC} )

SET( companion9x_SRCS
     eeprominterface.cpp
     hexinterface.cpp
     file.cpp
     # xmlinterface.cpp
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
