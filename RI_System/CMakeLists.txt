project( RI_System )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )
include_directories( include )
include_directories( . )

ADD_MSVC_PRECOMPILED_HEADER( "stdafx.h" "stdafx.cpp" SRCS )

# The following doesn't really work 8(
#source_group( "Sources" REGULAR_EXPRESSION src/.*\\.cpp|include/.*\\.h )
#source_group( "Sources\\Primitives" REGULAR_EXPRESSION src/RI_Primitive.*\\.cpp|include/RI_Primitive.*\\.h )

source_group( "Sources" FILES ${SRCS} ${INCS} )
source_group( "Precompiled Headers" FILES stdafx.cpp stdafx.h )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

target_link_libraries( ${PROJECT_NAME} DSystem DMath DImage )
