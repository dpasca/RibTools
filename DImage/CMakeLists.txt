project( DImage )

include_directories( ../externals/tiff/libtiff )

include_directories( ../externals/libjpeg )

include_directories( ../externals/local/libjpeg )
include_directories( ../externals/local/libtiff )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )
include_directories( include )
source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

target_link_libraries( ${PROJECT_NAME} DSystem libtiff )

if (BUILD_TESTING)
    add_executable(DImageTests tests/DImageTests.cpp)
    target_link_libraries(DImageTests PRIVATE DImage)
    add_test(NAME DImage.Core COMMAND DImageTests)
endif()
