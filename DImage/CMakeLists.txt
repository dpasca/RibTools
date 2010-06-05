project( DImage )

include_directories( ../externals/tiff-3.9.2/libtiff )

include_directories( ../externals/libjpeg )

include_directories( ../externals/local/libjpeg )
include_directories( ../externals/local/libtiff )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )
include_directories( include )
source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

