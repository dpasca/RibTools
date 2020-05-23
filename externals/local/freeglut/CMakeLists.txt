project( freeglut )

file( GLOB_RECURSE SRCS "../../freeglut/src/*.c" )
file( GLOB_RECURSE INCS "../../freeglut/src/*.h" )

include_directories( "../../freeglut/src" )
include_directories( "../../freeglut/include" )

source_group( Sources FILES ${SRCS} ${INCS} )

add_definitions( -DFREEGLUT_STATIC -DFREEGLUT_LIB_PRAGMAS=0 )

add_library( ${PROJECT_NAME} STATIC ${SRCS} ${INCS} )

if ( WIN32 )
target_link_libraries(
    ${PROJECT_NAME}
    opengl32
    winmm
    )
endif ( WIN32 )

