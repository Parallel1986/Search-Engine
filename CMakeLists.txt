cmake_minimum_required(VERSION 3.5)

project(Search-engine-GUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(GTESTS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

include(FetchContent)
if (GTESTS)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
endif()

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

if (GTESTS)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
endif()

set(PROJECT_SOURCES
		main.cpp
        mainwindow.h
        mainwindow.cpp
		ui/ui_main_window.cpp
		ui/ui_request_dialog.cpp
#       config_window.cpp
#       config_window.h
#		ui/ui_config_window.cpp
        source/converter_json.cpp
        source/inverted_index.cpp
        source/file_index.cpp
		source/requestadddialog.cpp
		include/requestadddialog.h
        include/converter_json.h
        include/inverted_index.h
        include/file_index.h
		include/engine_core.h
		include/exceptions.h
		source/engine_core.cpp
#		source/request_dialog.cpp

)

FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../config.json" DESTINATION "${CMAKE_BINARY_DIR}/")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../answers.json" DESTINATION "${CMAKE_BINARY_DIR}/")
FILE(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../requests.json" DESTINATION "${CMAKE_BINARY_DIR}/")



add_executable(Search-engine-GUI
			${PROJECT_SOURCES}
        )

if (GTESTS)
	target_link_libraries(Search-engine-GUI
		PRIVATE
		Qt5::Widgets
		PUBLIC
		nlohmann_json::nlohmann_json
		GTest::gtest_main
	)
else()
	target_link_libraries(Search-engine-GUI
		PRIVATE
		Qt5::Widgets
		PUBLIC
		nlohmann_json::nlohmann_json
	)
endif()

if (GTESTS)
add_executable(Search-engine-tests
        tests.cpp
        include/converter_json.h
        include/inverted_index.h
        include/file_index.h
        source/converter_json.cpp
        source/inverted_index.cpp
        source/file_index.cpp
)

target_link_libraries(Search-engine-tests PUBLIC
		nlohmann_json::nlohmann_json
        GTest::gtest_main
        Qt5::Core
)

include(GoogleTest)
gtest_discover_tests(Search-engine-tests)
endif()

