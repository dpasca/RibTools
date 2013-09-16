cmake_minimum_required (VERSION 2.8)

#=============================================
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
	GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
	SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
	SET(Sources ${${SourcesVar}})

	SET_SOURCE_FILES_PROPERTIES(
			${PrecompiledSource}
			PROPERTIES
			COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
			OBJECT_OUTPUTS "${PrecompiledBinary}")

	SET_SOURCE_FILES_PROPERTIES(
			${Sources}
			PROPERTIES
			COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
			OBJECT_DEPENDS "${PrecompiledBinary}")  

	# Add precompiled header to SourcesVar
	LIST(APPEND ${SourcesVar} ${PrecompiledSource})

  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

#=============================================
project (RibTools)

# if (CMAKE_COMPILER_IS_GNUCXX)
# 	ADD_DEFINITIONS(-std=c++0x) # Enable C++0x mode for unordered_map
# ENDIF()

# Find OpenMP and set the flags for the compiler
#find_package(OpenMP)
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

if(MSVC) #((((((((((((((((
    set(CMAKE_C_FLAGS_RELEASE   "/DNDEBUG /DWIN32     /MD  /Ox /Ot /Oi /GL /Ob2 /Oy /arch:SSE2")
    set(CMAKE_CXX_FLAGS_RELEASE "/DNDEBUG /DWIN32     /MD  /Ox /Ot /Oi /GL /Ob2 /Oy /arch:SSE2")
    set(CMAKE_C_FLAGS_DEBUG     "/D_DEBUG /DWIN32 /Zi /MDd /Od                      /arch:SSE2")
    set(CMAKE_CXX_FLAGS_DEBUG   "/D_DEBUG /DWIN32 /Zi /MDd /Od                      /arch:SSE2")
    # 4996: disable annoying secure CRT warnings
    # 4316: disable heap allocation may not align warning.. (we overload global new/delete)
    add_definitions( "/D_CRT_SECURE_NO_WARNINGS /wd4996 /wd4316 /nologo" )
    add_definitions( "/GR-" ) # disable RTTI
else()
    set(OSMACRO "")
    if(APPLE)
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(OSMACRO "-DMACOSX")
        else()
            set(OSMACRO "-DTARGET_OS_IPHONE")
        endif()
    endif()
    set(CMAKE_C_FLAGS   "${OSMACRO}")
    set(CMAKE_CXX_FLAGS "${OSMACRO} -std=c++11 -stdlib=libc++")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS}   -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS}   -D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -D_DEBUG")
endif() #))))))))))))))))

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "Release link flags")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG   "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"   CACHE STRING "Debug link flags")

# Add the source dir as a common include path
include_directories( . )
include_directories( DMath/include )
include_directories( DSystem/include )

# Specify the destination directly into the Distrib directory
# ..also avoid the /Release and /Debug additional subdirs created by
# Visual Studio
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Distrib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../Distrib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../Distrib )

# Various libs
add_subdirectory( DSystem )
add_subdirectory( DMath )
add_subdirectory( DImage )
add_subdirectory( RI_System )
add_subdirectory( RibRenderLib )
add_subdirectory( RibToolsBase )
add_subdirectory( RSLCompilerLib )

# external libs
add_subdirectory( externals/local/libtiff )
add_subdirectory( externals/local/libjpeg )
add_subdirectory( externals/local/freeglut )

# executables
add_subdirectory( RibRender )
add_subdirectory( RibRenderServer )
add_subdirectory( RibRenderToy )
add_subdirectory( RSLCompilerCmd )

