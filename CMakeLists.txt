cmake_minimum_required(VERSION 3.28)
project(clf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(clf ${SOURCE_FILES})
add_compile_options(-O3 -fno-omit-frame-pointer)

target_include_directories(clf PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")