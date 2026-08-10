project(RibToolsUI)

add_library(${PROJECT_NAME} STATIC
    include/RibToolsUI/ImGuiHost.h
    src/ImGuiHost.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC RibTools::DearImGui)

if (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE GL_SILENCE_DEPRECATION)
endif()
