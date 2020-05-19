project( RibRenderToy )

find_package(OpenGL)

file( GLOB_RECURSE SRCS "*.cpp" )
file( GLOB_RECURSE INCS "*.h" )
file( GLOB_RECURSE SRCSDD "../DispDrivers/src/*.cpp" )
file( GLOB_RECURSE INCSDD "../DispDrivers/include/*.h" )
include_directories( . )
include_directories( "../externals/freeglut/include" )
include_directories( "../DispDrivers/include" )
include_directories( ${OPENGL_INCLUDE_DIRS} )

source_group( Sources FILES ${SRCS} ${INCS} )
source_group( Sources\\DispDrivers FILES ${SRCSDD} ${INCSDD} )

add_definitions( -DFREEGLUT_STATIC -DFREEGLUT_LIB_PRAGMAS=0 )

add_executable( ${PROJECT_NAME} ${SRCS} ${INCS} ${SRCSDD} ${INCSDD} )

link_directories(
  ${OPENGL_LIBRARY_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	DSystem
	DMath
	DImage
	RI_System
	RibRenderLib
	RibToolsBase
	RSLCompilerLib
	freeglut
	libtiff
	libjpeg
	${OPENGL_LIBRARIES}
	)

if (MSVC)
    app_create_vcxproj_userfile("_distrib")
endif()
