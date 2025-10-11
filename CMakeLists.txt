cmake_minimum_required(VERSION 3.16)
project(qml-niri VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

add_library(niriplugin SHARED
    src/ipcclient.cpp
    src/niri.cpp
    src/plugin.cpp
    src/workspacemodel.cpp
)

target_link_libraries(niriplugin
    Qt6::Core
    Qt6::Qml
)

set_target_properties(niriplugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Niri
)

add_custom_command(TARGET niriplugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/qmldir
    ${CMAKE_BINARY_DIR}/Niri/qmldir
)
