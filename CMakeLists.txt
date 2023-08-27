cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SEARCH_ENGINE_GUI ON)

set (${SEARCH_ENGINE_GUI_RESOURCES} "")

if(SEARCH_ENGINE_GUI)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
        
    set (${SEARCH_ENGINE_GUI_RESOURCES}
            mainwindow.h
            mainwindow.cpp
            mainwindow.ui

        )
    find_package(Qt5 REQUIRED COMPONENTS Widgets)

endif()

project(Search-engine VERSION 0.1 LANGUAGES CXX)
project(Search-engine-tests VERSION 0.1 LANGUAGES CXX)

set(PROJECT_SOURCES        
        source/converter_json.cpp
        source/inverted_index.cpp
        source/file_index.cpp
)

set(PROJECT_INCLUDE_SOURCES 
    include/converter_json.h
    include/inverted_index.h
    include/file_index.h)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(Qt5 REQUIRED COMPONENTS Core)

FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/config.json" DESTINATION "${CMAKE_BINARY_DIR}/")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/answers.json" DESTINATION "${CMAKE_BINARY_DIR}/")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/requests.json" DESTINATION "${CMAKE_BINARY_DIR}/")

add_executable(Search-engine
          engine.cpp
          ${PROJECT_SOURCES}
          ${PROJECT_INCLUDE_SOURCES}
          ${SEARCH_ENGINE_GUI_RESOURCES}
)

add_executable(Search-engine-tests
          tests.cpp
          ${PROJECT_SOURCES}
          ${PROJECT_INCLUDE_SOURCES}
)



if(SEARCH_ENGINE_GUI)
    target_link_libraries(Search-engine PUBLIC
        nlohmann_json::nlohmann_json
        GTest::gtest_main
        Qt5::Core
        Qt5::Widgets
    )
else()
    target_link_libraries(Search-engine PUBLIC
            nlohmann_json::nlohmann_json
            GTest::gtest_main
            Qt5::Core
        )

endif()

target_link_libraries(Search-engine-tests PUBLIC
        nlohmann_json::nlohmann_json
        GTest::gtest_main
        Qt5::Core
)


include(GoogleTest)
gtest_discover_tests(Search-engine-tests)
