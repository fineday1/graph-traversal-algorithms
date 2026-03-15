cmake_minimum_required(VERSION 3.11)
project(GraphTraversalVisualizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Raylib
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG master
)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# Dear ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

# rlImGui
FetchContent_Declare(
  rlImGui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
  GIT_TAG main
)
FetchContent_GetProperties(rlImGui)
if(NOT rlimgui_POPULATED)
    FetchContent_Populate(rlImGui)
endif()

# Executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/Algorithm.cpp
    src/CMaze.cpp
    src/CGraph.cpp
    src/CGraph.hpp
    src/CVisualizerApp.cpp
    src/CVisualizerApp.hpp
    
    # ImGui sources
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    
    # rlImGui sources
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${rlimgui_SOURCE_DIR}
)

# Link everything
target_link_libraries(${PROJECT_NAME} PRIVATE 
    raylib
    GL
    X11
    m
    pthread
    dl
    rt
)
