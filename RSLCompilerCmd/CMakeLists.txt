project( RSLCompilerCmd )

file( GLOB_RECURSE SRCS "*.cpp" )
file( GLOB_RECURSE INCS "*.h" )
include_directories( . )

source_group( Sources FILES ${SRCS} ${INCS} )

add_executable( ${PROJECT_NAME} ${SRCS} ${INCS} )

target_link_libraries(
	${PROJECT_NAME}
	DSystem
	RibToolsBase
	RSLCompilerLib
	)

