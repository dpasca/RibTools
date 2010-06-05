cmake_minimum_required (VERSION 2.8)

#=============================================
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
	GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
	SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
	SET(Sources ${${SourcesVar}})

	SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
								PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
										   OBJECT_OUTPUTS "${PrecompiledBinary}")
	SET_SOURCE_FILES_PROPERTIES(${Sources}
								PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
										   OBJECT_DEPENDS "${PrecompiledBinary}")  
	# Add precompiled header to SourcesVar
	LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

#=============================================
project (RibTools)

add_definitions( "/D_CRT_SECURE_NO_WARNINGS /wd4996 /nologo" )

include_directories( . )

add_subdirectory( DSystem )
add_subdirectory( DMath )
add_subdirectory( DImage )
add_subdirectory( externals/local/libtiff )
add_subdirectory( externals/local/libjpeg )

add_subdirectory( externals/local/freeglut )

add_subdirectory( RI_System )
add_subdirectory( RibRenderLib )
add_subdirectory( RibToolsBase )
add_subdirectory( RSLCompilerLib )

add_subdirectory( RibRender )

