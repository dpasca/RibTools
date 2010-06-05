project( DMath )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )

include_directories( include )

source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

