cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_COMPILER g++)
project(clf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SOURCE_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_executable(clf ${SOURCE_FILES})
target_compile_options(clf PRIVATE -Wpsabi -mavx2 -O3 -flto=auto -fno-omit-frame-pointer -march=native)

target_include_directories(clf PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
