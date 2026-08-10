project( DSystem )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )

include_directories( include )

source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

if (BUILD_TESTING)
    add_executable(DSystemSafeCrtTests tests/DSafeCrtTests.cpp)
    target_link_libraries(DSystemSafeCrtTests PRIVATE DSystem)
    add_test(NAME DSystem.SafeCrt COMMAND DSystemSafeCrtTests)
endif()
