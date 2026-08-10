project( DImage )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )
include_directories( include )
source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC DSystem
    PRIVATE RibTools::JPEG TIFF::tiff)

if (BUILD_TESTING)
    add_executable(DImageTests tests/DImageTests.cpp)
    target_link_libraries(DImageTests PRIVATE DImage RibTools::JPEG)
    add_test(NAME DImage.Core COMMAND DImageTests "${CMAKE_CURRENT_BINARY_DIR}")
endif()
