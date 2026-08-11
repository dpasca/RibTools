project( RSLCompilerLib )

file( GLOB_RECURSE SRCS "src/*.cpp" )
file( GLOB_RECURSE INCS "include/*.h" )
include_directories( include )

source_group( Sources FILES ${SRCS} ${INCS} )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

if (BUILD_TESTING)
    add_executable(RSLCompilerTokenCalcTests tests/TokenCalcTests.cpp)
    target_link_libraries(RSLCompilerTokenCalcTests PRIVATE RSLCompilerLib DSystem)
    add_test(NAME RSLCompiler.TokenCalc COMMAND RSLCompilerTokenCalcTests)

    add_executable(RSLCompilerPreproTests tests/PreproTests.cpp)
    target_link_libraries(RSLCompilerPreproTests PRIVATE RSLCompilerLib DSystem)
    add_test(NAME RSLCompiler.Preprocessor COMMAND RSLCompilerPreproTests)
endif()
