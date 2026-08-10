project( RibRenderToy )

file( GLOB_RECURSE SRCS "*.cpp" )
file( GLOB_RECURSE INCS "*.h" )
file( GLOB_RECURSE SRCSDD "../DispDrivers/src/*.cpp" )
file( GLOB_RECURSE INCSDD "../DispDrivers/include/*.h" )
include_directories( . )
include_directories( "../DispDrivers/include" )

source_group( Sources FILES ${SRCS} ${INCS} )
source_group( Sources\\DispDrivers FILES ${SRCSDD} ${INCSDD} )

add_executable( ${PROJECT_NAME} ${SRCS} ${INCS} ${SRCSDD} ${INCSDD} )

if (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE FREEGLUT_LIB_PRAGMAS=0)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    DSystem
    DMath
    DImage
    RI_System
    RibRenderLib
    RibToolsBase
    RSLCompilerLib
    ${RIBTOOLS_GLUT_TARGET}
    libtiff
    libjpeg
    OpenGL::GL
    OpenGL::GLU
    )

if (MSVC)
    app_create_vcxproj_userfile("_distrib")
endif()
