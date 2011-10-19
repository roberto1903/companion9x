FIND_PACKAGE( Subversion )

SET( C9X_WC_REVISION 0 )

IF( Subversion_FOUND )
  Subversion_WC_INFO( ${SOURCE_DIR} C9X )
ENDIF( )

FILE( WRITE svnrevision.h.txt "#define C9X_REVISION ${C9X_WC_REVISION}\n")
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
      svnrevision.h.txt svnrevision.h)