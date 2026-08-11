cmake_minimum_required(VERSION 3.21)

if (POLICY CMP0156)
    cmake_policy(SET CMP0156 NEW)
endif()

if (POLICY CMP0179)
    cmake_policy(SET CMP0179 NEW)
endif()

project(RibTools VERSION 0.11.0 LANGUAGES C CXX)

include(CTest)

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

# create vcproj.user file for Visual Studio to set debug working directory
function(app_create_vcxproj_userfile extraPath)
    if (MSVC)
        set(APP_WORKING_DIR "${PROJECT_SOURCE_DIR}/../${extraPath}")
        configure_file(
              ${CMAKE_SOURCE_DIR}/DebugDir.vcxproj.user.in
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user
              @ONLY
            )
    endif()
endfunction()

# Keep the applications and their runtime assets together. This also avoids
# the per-configuration subdirectories normally added by Visual Studio.
set(RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_SOURCE_DIR}/_distrib"
    CACHE PATH "Directory for RibTools executables")
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}" )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(
    RIBTOOLS_ENABLE_SANITIZERS
    "Enable AddressSanitizer and UndefinedBehaviorSanitizer instrumentation"
    OFF)

if (RIBTOOLS_ENABLE_SANITIZERS)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        add_compile_options(
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
            -fno-sanitize-recover=all)
        add_link_options(
            -fsanitize=address,undefined
            -fno-sanitize-recover=all)
    else()
        message(
            FATAL_ERROR
            "RIBTOOLS_ENABLE_SANITIZERS requires a Clang- or GCC-compatible compiler")
    endif()
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

# Image codecs are pinned submodules. See cmake/RibToolsDependencies.cmake.
include(cmake/RibToolsDependencies.cmake)

add_subdirectory( DImage )
add_subdirectory( RSLCompilerLib )
add_subdirectory( RI_System )
add_subdirectory( RibRenderLib )
add_subdirectory( RibToolsBase )

find_package(OpenGL REQUIRED)
include(cmake/RibToolsUI.cmake)
add_subdirectory(RibToolsUI)

# executables
add_subdirectory( RibRender )
add_subdirectory( RibRenderServer )
add_subdirectory( RibRenderToy )
add_subdirectory( RSLCompilerCmd )

# Stage the data needed to run RibTools as part of every normal build. This is
# the cross-platform replacement for the old make_distrib shell and batch
# scripts; generated shader caches remain in place between builds.
set(RIBTOOLS_RUNTIME_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/DistribSrc/base")
add_custom_target(
    RibToolsRuntimeAssets ALL
    COMMAND
        "${CMAKE_COMMAND}" -E make_directory
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}"
    COMMAND
        "${CMAKE_COMMAND}" -E copy_directory
        "${RIBTOOLS_RUNTIME_SOURCE_DIRECTORY}"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}"
    COMMAND
        "${CMAKE_COMMAND}" -E rm -f
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/rendermn.ini"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/readme.html"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/readme.txt"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/make_tests.bat"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/make_tests.sh"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/del_compiled_shaders.bat"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/del_compiled_shaders.sh"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/TestScenes/MiscTests/render.bat"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/TestScenes/MiscTests/render.sh"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/TestScenes/ValuesTests/render.bat"
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/TestScenes/ValuesTests/render_aqsis.bat"
    COMMAND
        "${CMAKE_COMMAND}" -E remove_directory
        "${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}/Extras"
    COMMENT "Staging RibTools runtime assets")

foreach(APP_TARGET RibRender RibRenderServer RibRenderToy RSLCompilerCmd)
    add_dependencies(${APP_TARGET} RibToolsRuntimeAssets)
endforeach()

add_custom_target(
    RibToolsCleanShaderCache
    COMMAND
        "${CMAKE_COMMAND}"
        "-DRIBTOOLS_RUNTIME_DIRECTORY=${RIBTOOLS_RUNTIME_OUTPUT_DIRECTORY}"
        -P "${CMAKE_SOURCE_DIR}/cmake/CleanRuntimeShaderCache.cmake"
    COMMENT "Removing generated runtime shader caches")

# CPack installs into a fresh temporary tree, so release archives cannot pick
# up stale render outputs, settings, or generated shader caches from _distrib.
install(
    TARGETS RibRender RibRenderServer RibRenderToy RSLCompilerCmd
    RUNTIME DESTINATION .)
install(
    DIRECTORY
        "${RIBTOOLS_RUNTIME_SOURCE_DIRECTORY}/Resources"
        "${RIBTOOLS_RUNTIME_SOURCE_DIRECTORY}/TestScenes"
    DESTINATION .
    PATTERN "Unused" EXCLUDE
    PATTERN "*.autogen.rrasm" EXCLUDE
    PATTERN "*.slx" EXCLUDE)
install(
    FILES
        "${RIBTOOLS_RUNTIME_SOURCE_DIRECTORY}/ribtools.ini"
        "${CMAKE_SOURCE_DIR}/README.md"
        "${CMAKE_SOURCE_DIR}/license.txt"
    DESTINATION .)
install(
    FILES "${CMAKE_SOURCE_DIR}/docs/images/rib-render-toy-killeroo.png"
    DESTINATION docs/images)

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_NAME "RibTools")
set(CPACK_PACKAGE_VENDOR "Davide Pasca")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Experimental RenderMan-style REYES renderer")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}/packages")
set(
    CPACK_PACKAGE_FILE_NAME
    "RibTools-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/license.txt")
include(CPack)

if (BUILD_TESTING)
    add_executable(
        RISystemShaderCompilerTests
        RI_System/tests/ShaderCompilerTests.cpp)
    target_link_libraries(
        RISystemShaderCompilerTests
        PRIVATE RI_System RSLCompilerLib)

    add_executable(
        RIArrayVisualTests
        RI_System/tests/ArrayVisualTests.cpp)
    target_link_libraries(
        RIArrayVisualTests
        PRIVATE DImage DSystem)

    add_executable(
        RIKillerooVisualTests
        RI_System/tests/KillerooVisualTests.cpp)
    target_link_libraries(
        RIKillerooVisualTests
        PRIVATE DImage DSystem)

    add_executable(
        RITextureTests
        RI_System/tests/TextureTests.cpp)
    target_link_libraries(
        RITextureTests
        PRIVATE RI_System)

    add_executable(
        RICameraOverrideTests
        RI_System/tests/CameraOverrideTests.cpp)
    target_link_libraries(
        RICameraOverrideTests
        PRIVATE RibRenderLib)

    set(
        RIBTOOLS_CAMERA_TEST_SHADER_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}/camera-override-shaders")
    foreach(
        SHADER_FILE
        IN ITEMS
            RSLC_Builtins.sl
            constant.sl
            dbg_normal_col.rrasm
            matte.sl)
        configure_file(
            "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/${SHADER_FILE}"
            "${RIBTOOLS_CAMERA_TEST_SHADER_DIRECTORY}/${SHADER_FILE}"
            COPYONLY)
    endforeach()

    set(RIBTOOLS_SHADER_CONTRACT_SOURCES
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/matte_fat.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/matte.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test01.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test02.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test04.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test05.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test06.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test07.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test08.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test09.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test10_1.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test11.sl"
        "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/unary_operators.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/ambientlight.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/arealight.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/constant.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/distantlight.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/matte.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/metal.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/plastic.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/pointlight.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/KillerooNURBS/Shaders/disp_textured.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/KillerooNURBS/Shaders/shoulderlight.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/ArrayTests/array_red_green.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/MiscTests/T01_TestIfElse.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/SpacesTests/SpacesTest1Sha.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/SpacesTests/SpacesTestCameraSha.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/SpacesTests/SpacesTestObjectSha.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/SpacesTests/SpacesTestRasterSha.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/SpacesTests/SpacesTestScreenSha.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/SpacesTests/SpacesTestWorldSha.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/ValuesTests/dbg_show_I.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/ValuesTests/dbg_show_N.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/ValuesTests/dbg_show_negI.sl"
        "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/ValuesTests/dbg_show_Nf.sl")

    add_test(
        NAME RI_System.ShaderCompilerContract
        COMMAND RISystemShaderCompilerTests
            "${CMAKE_CURRENT_BINARY_DIR}/RI_System/shader-contract"
            "${CMAKE_SOURCE_DIR}/DistribSrc/base/Resources/Shaders/RSLC_Builtins.sl"
            ${RIBTOOLS_SHADER_CONTRACT_SOURCES})

    add_test(NAME RibRender.Help COMMAND RibRender --help)
    add_test(NAME RibRenderToy.Help COMMAND RibRenderToy --help)

    add_test(
        NAME RI_System.TextureSampling
        COMMAND RITextureTests
            "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/KillerooNURBS/Maps-new/pat006_0.4.mip")
    add_test(
        NAME RI_System.CameraOverride
        COMMAND RICameraOverrideTests
            "${RIBTOOLS_CAMERA_TEST_SHADER_DIRECTORY}")

    add_test(NAME RSLCompilerCmd.Help COMMAND RSLCompilerCmd --help)

    add_test(
        NAME RSLCompilerCmd.ShaderSmoke
        COMMAND RSLCompilerCmd
            "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/matte.sl"
            "${CMAKE_CURRENT_BINARY_DIR}/matte.rrasm")
    set_tests_properties(
        RSLCompilerCmd.ShaderSmoke
        PROPERTIES ENVIRONMENT "RIBTOOLS_DIR=${CMAKE_SOURCE_DIR}/DistribSrc/base")

    add_test(
        NAME RSLCompilerCmd.PreprocessorShader
        COMMAND RSLCompilerCmd
            "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test10_1.sl"
            "${CMAKE_CURRENT_BINARY_DIR}/test10_1.rrasm")
    set_tests_properties(
        RSLCompilerCmd.PreprocessorShader
        PROPERTIES ENVIRONMENT "RIBTOOLS_DIR=${CMAKE_SOURCE_DIR}/DistribSrc/base")

    add_test(
        NAME RSLCompilerCmd.UnaryOperators
        COMMAND RSLCompilerCmd
            "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/unary_operators.sl"
            "${CMAKE_CURRENT_BINARY_DIR}/unary_operators.rrasm")
    add_test(
        NAME RSLCompilerCmd.UnaryShoulderLight
        COMMAND RSLCompilerCmd
            "${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/KillerooNURBS/Shaders/shoulderlight.sl"
            "${CMAKE_CURRENT_BINARY_DIR}/shoulderlight.rrasm")
    add_test(
        NAME RSLCompilerCmd.ArrayFrontEnd
        COMMAND RSLCompilerCmd
            "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/test11.sl"
            "${CMAKE_CURRENT_BINARY_DIR}/test11.rrasm")
    set_tests_properties(
        RSLCompilerCmd.UnaryOperators
        RSLCompilerCmd.UnaryShoulderLight
        RSLCompilerCmd.ArrayFrontEnd
        PROPERTIES ENVIRONMENT "RIBTOOLS_DIR=${CMAKE_SOURCE_DIR}/DistribSrc/base")

    add_test(
        NAME RSLCompilerCmd.RejectsInvalidShader
        COMMAND RSLCompilerCmd
            "${CMAKE_SOURCE_DIR}/RSLCompilerCmd/tests/invalid.sl"
            "${CMAKE_CURRENT_BINARY_DIR}/invalid.rrasm")
    set_tests_properties(
        RSLCompilerCmd.RejectsInvalidShader
        PROPERTIES
            ENVIRONMENT "RIBTOOLS_DIR=${CMAKE_SOURCE_DIR}/DistribSrc/base"
            WILL_FAIL TRUE)

    add_test(
        NAME RI_System.ArrayVisualRender
        COMMAND "${CMAKE_COMMAND}"
            "-DRENDERER=$<TARGET_FILE:RibRender>"
            "-DSCENE=${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/ArrayTests/array_red_green.rib"
            "-DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/array_red_green.tiff"
            "-DRIBTOOLS_DIR=${CMAKE_SOURCE_DIR}/DistribSrc/base"
            "-DTEST_NAME=array-visual"
            -P "${CMAKE_SOURCE_DIR}/RI_System/tests/RunVisualRenderTest.cmake")
    set_tests_properties(
        RI_System.ArrayVisualRender
        PROPERTIES
            FIXTURES_SETUP RIArrayVisual
            TIMEOUT 30)

    add_test(
        NAME RI_System.ArrayVisualCheck
        COMMAND RIArrayVisualTests
            "${CMAKE_CURRENT_BINARY_DIR}/array_red_green.tiff")
    set_tests_properties(
        RI_System.ArrayVisualCheck
        PROPERTIES
            DEPENDS RI_System.ArrayVisualRender
            FIXTURES_REQUIRED RIArrayVisual
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    add_test(
        NAME RI_System.KillerooVisualRender
        COMMAND "${CMAKE_COMMAND}"
            "-DRENDERER=$<TARGET_FILE:RibRender>"
            "-DSCENE=${CMAKE_SOURCE_DIR}/DistribSrc/base/TestScenes/KillerooNURBS/killeroo.rib"
            "-DOUTPUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/killeroo.0001.tif"
            "-DRIBTOOLS_DIR=${CMAKE_SOURCE_DIR}/DistribSrc/base"
            "-DTEST_NAME=killeroo-visual"
            "-DFORCED_LONG_DIM=160"
            -P "${CMAKE_SOURCE_DIR}/RI_System/tests/RunVisualRenderTest.cmake")
    set_tests_properties(
        RI_System.KillerooVisualRender
        PROPERTIES
            FIXTURES_SETUP RIKillerooVisual
            TIMEOUT 60)

    add_test(
        NAME RI_System.KillerooVisualCheck
        COMMAND RIKillerooVisualTests
            "${CMAKE_CURRENT_BINARY_DIR}/killeroo.0001.tif")
    set_tests_properties(
        RI_System.KillerooVisualCheck
        PROPERTIES
            DEPENDS RI_System.KillerooVisualRender
            FIXTURES_REQUIRED RIKillerooVisual
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    if (UNIX AND NOT APPLE)
        find_program(RIBTOOLS_XVFB_RUN xvfb-run)
        if (RIBTOOLS_XVFB_RUN)
            add_test(
                NAME RibRenderToy.UiSmoke
                COMMAND "${RIBTOOLS_XVFB_RUN}" -a
                    "$<TARGET_FILE:RibRenderToy>" --smoke-test)
            set_tests_properties(
                RibRenderToy.UiSmoke
                PROPERTIES
                    ENVIRONMENT "RIBTOOLS_DIR=${CMAKE_SOURCE_DIR}/DistribSrc/base"
                    TIMEOUT 30)
        endif()
    endif()
endif()
