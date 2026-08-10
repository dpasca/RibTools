include(ExternalProject)

set(RIBTOOLS_LIBJPEG_SOURCE_DIR "${CMAKE_SOURCE_DIR}/externals/libjpeg-turbo")
set(RIBTOOLS_LIBTIFF_SOURCE_DIR "${CMAKE_SOURCE_DIR}/externals/libtiff")

if (NOT EXISTS "${RIBTOOLS_LIBJPEG_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "The libjpeg-turbo submodule is missing. Run: git submodule update --init --recursive")
endif()

if (NOT EXISTS "${RIBTOOLS_LIBTIFF_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "The libtiff submodule is missing. Run: git submodule update --init --recursive")
endif()

# libjpeg-turbo deliberately does not support add_subdirectory(), so build its
# pinned source tree using the upstream-supported ExternalProject route.
set(RIBTOOLS_LIBJPEG_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/_deps/libjpeg-turbo")
set(RIBTOOLS_LIBJPEG_INSTALL_DIR "${RIBTOOLS_LIBJPEG_PREFIX}/install")
set(RIBTOOLS_LIBJPEG_CMAKE_ARGS
    "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
    "-DCMAKE_INSTALL_LIBDIR:STRING=lib"
    "-DENABLE_SHARED:BOOL=OFF"
    "-DENABLE_STATIC:BOOL=ON"
    "-DWITH_SIMD:BOOL=OFF"
    "-DWITH_TURBOJPEG:BOOL=OFF"
    "-DWITH_TOOLS:BOOL=OFF"
    "-DWITH_TESTS:BOOL=OFF")

if (CMAKE_TOOLCHAIN_FILE)
    list(APPEND RIBTOOLS_LIBJPEG_CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}")
endif()

if (CMAKE_OSX_ARCHITECTURES)
    list(APPEND RIBTOOLS_LIBJPEG_CMAKE_ARGS
        "-DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES}")
endif()

if (CMAKE_OSX_DEPLOYMENT_TARGET)
    list(APPEND RIBTOOLS_LIBJPEG_CMAKE_ARGS
        "-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET}")
endif()

if (CMAKE_MSVC_RUNTIME_LIBRARY)
    list(APPEND RIBTOOLS_LIBJPEG_CMAKE_ARGS
        "-DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=${CMAKE_MSVC_RUNTIME_LIBRARY}")
endif()

if (MSVC)
    set(RIBTOOLS_LIBJPEG_LIBRARY
        "${RIBTOOLS_LIBJPEG_INSTALL_DIR}/lib/jpeg-static${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
    set(RIBTOOLS_LIBJPEG_LIBRARY
        "${RIBTOOLS_LIBJPEG_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

file(MAKE_DIRECTORY "${RIBTOOLS_LIBJPEG_INSTALL_DIR}/include")

ExternalProject_Add(RibToolsLibJpegTurbo
    PREFIX "${RIBTOOLS_LIBJPEG_PREFIX}"
    SOURCE_DIR "${RIBTOOLS_LIBJPEG_SOURCE_DIR}"
    BINARY_DIR "${RIBTOOLS_LIBJPEG_PREFIX}/build"
    INSTALL_DIR "${RIBTOOLS_LIBJPEG_INSTALL_DIR}"
    CMAKE_ARGS
        "-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>"
        ${RIBTOOLS_LIBJPEG_CMAKE_ARGS}
    UPDATE_COMMAND ""
    TEST_COMMAND ""
    BUILD_ALWAYS TRUE
    BUILD_BYPRODUCTS "${RIBTOOLS_LIBJPEG_LIBRARY}")

add_library(RibToolsJPEG STATIC IMPORTED GLOBAL)
set_target_properties(RibToolsJPEG PROPERTIES
    IMPORTED_LOCATION "${RIBTOOLS_LIBJPEG_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${RIBTOOLS_LIBJPEG_INSTALL_DIR}/include")
add_dependencies(RibToolsJPEG RibToolsLibJpegTurbo)
add_library(RibTools::JPEG ALIAS RibToolsJPEG)

# RibTools only needs the core, built-in TIFF codecs. Keep the embedded build
# static and independent of optional system compression libraries.
set(tiff-static ON CACHE BOOL "Build a static libtiff library")
set(tiff-tools OFF CACHE BOOL "Build libtiff command-line tools")
set(tiff-tests OFF CACHE BOOL "Build libtiff tests")
set(tiff-contrib OFF CACHE BOOL "Build libtiff contributed tools")
set(tiff-docs OFF CACHE BOOL "Build libtiff documentation")
set(tiff-install OFF CACHE BOOL "Install libtiff targets")
set(tiff-cxx OFF CACHE BOOL "Build the libtiff C++ stream API")
set(tiff-opengl OFF CACHE BOOL "Build libtiff OpenGL support")
set(zlib OFF CACHE BOOL "Build libtiff with zlib support")
set(libdeflate OFF CACHE BOOL "Build libtiff with libdeflate support")
set(pixarlog OFF CACHE BOOL "Build libtiff with PixarLog support")
set(jpeg OFF CACHE BOOL "Build libtiff with JPEG support")
set(old-jpeg OFF CACHE BOOL "Build libtiff with old JPEG support")
set(jbig OFF CACHE BOOL "Build libtiff with JBIG support")
set(lerc OFF CACHE BOOL "Build libtiff with LERC support")
set(lzma OFF CACHE BOOL "Build libtiff with LZMA support")
set(zstd OFF CACHE BOOL "Build libtiff with Zstandard support")
set(webp OFF CACHE BOOL "Build libtiff with WebP support")

set(RIBTOOLS_DISABLED_TIFF_PACKAGES
    ZLIB
    Deflate
    JPEG
    JBIG
    LERC
    libjpeg-turbo
    liblzma
    ZSTD
    WebP)
foreach(PACKAGE_NAME IN LISTS RIBTOOLS_DISABLED_TIFF_PACKAGES)
    set(CMAKE_DISABLE_FIND_PACKAGE_${PACKAGE_NAME} TRUE)
endforeach()

add_subdirectory("${RIBTOOLS_LIBTIFF_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/_deps/libtiff")

foreach(PACKAGE_NAME IN LISTS RIBTOOLS_DISABLED_TIFF_PACKAGES)
    unset(CMAKE_DISABLE_FIND_PACKAGE_${PACKAGE_NAME})
endforeach()
unset(PACKAGE_NAME)
