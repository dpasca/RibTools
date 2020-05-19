project( RibRenderLib )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )
include_directories( include )
include_directories( . )

ADD_MSVC_PRECOMPILED_HEADER( "stdafx.h" "stdafx.cpp" SRCS )

source_group( Sources FILES ${SRCS} ${INCS} )
source_group( "Precompiled Headers" FILES stdafx.cpp stdafx.h )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

