PROJECT( eepe )

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

SET( eepe_SRCS
     eeprominterface.cpp
     hexinterface.cpp
     file.cpp
     # xmlinterface.cpp
     ${PROJECT_BINARY_DIR}/radio.cxx
     er9xeeprom.cpp
     er9xinterface.cpp
     #gruvin9xeeprom.cpp
     #gruvin9xinterface.cpp  
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

SET( eepe_MOC_HDRS
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
     
SET( eepe_UIS
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

SET( eepe_RCS
     eepe.qrc
)

SET( eepe_TS
     eepe_he.ts
     eepe_pt.ts
     eepe_ru.ts
     eepe_de.ts
     eepe_fr.ts
)

FIND_PACKAGE( Qt4 REQUIRED )
SET( QT_USE_QTNETWORK True )
INCLUDE( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

QT4_WRAP_UI( eepe_SRCS ${eepe_UIS} )
QT4_WRAP_CPP( eepe_SRCS ${eepe_MOC_HDRS} )
QT4_ADD_TRANSLATION( eepe_QM ${eepe_TS} )
QT4_ADD_RESOURCES( eepe_SRCS ${eepe_RCS} )

ADD_EXECUTABLE( eepe ${eepe_SRCS} ${eepe_QM} )
TARGET_LINK_LIBRARIES( eepe ${QT_LIBRARIES} )
TARGET_LINK_LIBRARIES( eepe xerces-c )
 
INSTALL( TARGETS eepe DESTINATION ${QT_BINARY_DIR} )
