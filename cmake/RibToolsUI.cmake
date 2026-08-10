set(RIBTOOLS_IMGUI_SOURCE_DIR "${CMAKE_SOURCE_DIR}/externals/imgui")
set(RIBTOOLS_GLFW_SOURCE_DIR "${CMAKE_SOURCE_DIR}/externals/glfw")

if (NOT EXISTS "${RIBTOOLS_IMGUI_SOURCE_DIR}/imgui.cpp")
    message(FATAL_ERROR
        "The Dear ImGui submodule is missing. Run: git submodule update --init --recursive")
endif()

if (NOT EXISTS "${RIBTOOLS_GLFW_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "The GLFW submodule is missing. Run: git submodule update --init --recursive")
endif()

set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "Build GLFW as a static library" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Disable GLFW documentation" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Disable GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Disable GLFW tests" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Disable GLFW installation" FORCE)

if (UNIX AND NOT APPLE)
    # Dear ImGui's detached platform windows require window positioning, which
    # Wayland intentionally does not expose. Keep the supported X11 backend.
    set(GLFW_BUILD_X11 ON CACHE BOOL "Build GLFW with X11 support" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable GLFW Wayland support" FORCE)
endif()

add_subdirectory(
    "${RIBTOOLS_GLFW_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/_deps/glfw"
    EXCLUDE_FROM_ALL)

add_library(RibToolsDearImGui STATIC
    "${RIBTOOLS_IMGUI_SOURCE_DIR}/imgui.cpp"
    "${RIBTOOLS_IMGUI_SOURCE_DIR}/imgui_draw.cpp"
    "${RIBTOOLS_IMGUI_SOURCE_DIR}/imgui_tables.cpp"
    "${RIBTOOLS_IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
    "${RIBTOOLS_IMGUI_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
    "${RIBTOOLS_IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${RIBTOOLS_IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl2.cpp")

add_library(RibTools::DearImGui ALIAS RibToolsDearImGui)

target_include_directories(RibToolsDearImGui
    SYSTEM PUBLIC
        "${RIBTOOLS_IMGUI_SOURCE_DIR}"
        "${RIBTOOLS_IMGUI_SOURCE_DIR}/backends"
        "${RIBTOOLS_IMGUI_SOURCE_DIR}/misc/cpp")

target_compile_definitions(RibToolsDearImGui PUBLIC GLFW_INCLUDE_NONE)
target_link_libraries(RibToolsDearImGui PUBLIC glfw OpenGL::GL)
