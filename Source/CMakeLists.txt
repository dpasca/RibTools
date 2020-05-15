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

#==================================================================
# Add new build types
message("* Adding build types...")
SET(CMAKE_CXX_FLAGS_DEBUGOPT
    "${CMAKE_CXX_FLAGS_DEBUG}"
    CACHE STRING "Flags used by the C++ compiler during DebugOpt builds."
    FORCE )
SET(CMAKE_C_FLAGS_DEBUGOPT
    "${CMAKE_C_FLAGS_DEBUG}"
    CACHE STRING "Flags used by the C compiler during DebugOpt builds."
    FORCE )
SET(CMAKE_EXE_LINKER_FLAGS_DEBUGOPT
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"
    CACHE STRING "Flags used for linking binaries during DebugOpt builds."
    FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_DEBUGOPT
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}"
    CACHE STRING "Flags used by the shared libraries linker during DebugOpt builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_DEBUGOPT
    CMAKE_C_FLAGS_DEBUGOPT
    CMAKE_EXE_LINKER_FLAGS_DEBUGOPT
    CMAKE_SHARED_LINKER_FLAGS_DEBUGOPT )

if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES Debug Release DebugOpt)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
    "Reset the configurations to what we need"
    FORCE)
endif()

# sepcify which are the "debug" configs (for the linker)
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug;DebugOpt")

#=============================================
project (RibTools)

# create vcproj.user file for Visual Studio to set debug working directory
function(app_create_vcxproj_userfile extraPath)
    if (MSVC)
        set(APP_WORKING_DIR "${PROJECT_SOURCE_DIR}/../../${extraPath}")
        configure_file(
              ${CMAKE_SOURCE_DIR}/DebugDir.vcxproj.user.in
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user
              @ONLY
            )
    endif()
endfunction()

# Specify the destination directly into the Distrib directory
# ..also avoid the /Release and /Debug additional subdirs created by
# Visual Studio
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Distrib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../Distrib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../Distrib )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CPPFS_LIBRARIES "stdc++fs")
endif()

#
if (MSVC)
    set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG}       /DDEBUG /D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}     /DDEBUG /D_DEBUG")
    set(CMAKE_C_FLAGS_DEBUGOPT    "${CMAKE_C_FLAGS_DEBUGOPT}    /DDEBUG /D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUGOPT  "${CMAKE_CXX_FLAGS_DEBUGOPT}  /DDEBUG /D_DEBUG")
else()
    set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG}       -DDEBUG -D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}     -DDEBUG -D_DEBUG")
    set(CMAKE_C_FLAGS_DEBUGOPT    "${CMAKE_C_FLAGS_DEBUGOPT}    -DDEBUG -D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUGOPT  "${CMAKE_CXX_FLAGS_DEBUGOPT}  -DDEBUG -D_DEBUG")
endif()

if (MSVC)
    # disable annoying secure CRT warnings
    add_definitions( "/D_CRT_SECURE_NO_WARNINGS /wd4996 /nologo" )
endif()

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE     "${CMAKE_C_FLAGS_RELEASE}     /Ox /Ot /Oi /Ob2 /Oy /GS-")
    set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE}   /Ox /Ot /Oi /Ob2 /Oy /GS-")
    set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG}       /Od /Zi")
    set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}     /Od /Zi")
    set(CMAKE_C_FLAGS_DEBUGOPT    "${CMAKE_C_FLAGS_DEBUGOPT}    /O2 /Zi /Ot /Oi /Ob2")
    set(CMAKE_CXX_FLAGS_DEBUGOPT  "${CMAKE_CXX_FLAGS_DEBUGOPT}  /O2 /Zi /Ot /Oi /Ob2")

    # reduce burden on iterators debugging
    set(CMAKE_C_FLAGS_DEBUG       "${CMAKE_C_FLAGS_DEBUG}       /D_ITERATOR_DEBUG_LEVEL=1")
    set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}     /D_ITERATOR_DEBUG_LEVEL=1")
    set(CMAKE_C_FLAGS_DEBUGOPT    "${CMAKE_C_FLAGS_DEBUGOPT}    /D_ITERATOR_DEBUG_LEVEL=1")
    set(CMAKE_CXX_FLAGS_DEBUGOPT  "${CMAKE_CXX_FLAGS_DEBUGOPT}  /D_ITERATOR_DEBUG_LEVEL=1")

    string( REPLACE "/RTC1" ""  CMAKE_C_FLAGS_DEBUGOPT    ${CMAKE_C_FLAGS_DEBUGOPT} )
    string( REPLACE "/RTC1" ""  CMAKE_CXX_FLAGS_DEBUGOPT  ${CMAKE_CXX_FLAGS_DEBUGOPT} )

    add_definitions( "/fp:fast" )            # common params for FPU
    add_definitions( "/DWIN32" ) # some common params
    add_definitions( "/DNOMINMAX" )          # avoid crazy old Windows.h min and max macros
    add_definitions( "/MP" )                 # multi-process compilation
    add_definitions( "/we4239" )
endif()

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "Release link flags")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG   "${CMAKE_EXE_LINKER_FLAGS_DEBUG}"   CACHE STRING "Debug link flags")

# Add the source dir as a common include path
include_directories( . )
include_directories( DMath/include )
include_directories( DSystem/include )

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

