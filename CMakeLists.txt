cmake_minimum_required(VERSION 3.12)

project(CellulAutomata)

set(CMAKE_CXX_STANDARD 17)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

file(GLOB RES "./res" )

file(COPY ${RES} DESTINATION ${CMAKE_BINARY_DIR})

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Dependences/include)
#include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/src/mainApplication)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Application)

#link_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Dependences/lib)

add_subdirectory(Application)
add_subdirectory(src/mainApplication)

add_executable(CellulAutomata "src/main.cpp")

list(APPEND compile_flag "-Wall" "-O3" )

target_compile_options(CellulAutomata PUBLIC ${compile_flag})

target_link_libraries(CellulAutomata mainApplication SDL2main SDL2 SDL2_test app  SDL2_image SDL2_ttf SDL2_mixer) 

