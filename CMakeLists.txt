cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)

project(Search-engine VERSION 0.1 LANGUAGES CXX)

set(PROJECT_SOURCES
        engine.cpp
        source/converter_json.cpp
        source/inverted_index.cpp
        source/file_index.cpp
)

set(PROJECT_INCLUDE_SOURCES 
    include/converter_json.h
    include/inverted_index.h
    include/file_index.h)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

#FetchContent_Declare(
#googletest
#URL
#https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)

#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


set(gtest_disable_pthreads on) #for MinGW
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

#FetchContent_MakeAvailable(googletest)

enable_testing()


add_executable(Search-engine
          ${PROJECT_SOURCES}
          ${PROJECT_INCLUDE_SOURCES}
)

target_link_libraries(Search-engine PRIVATE
            #gtest_main
            nlohmann_json::nlohmann_json
    )

#include(GoogleTest)
#gtest_discover_tests(Search-engine)
