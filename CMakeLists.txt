cmake_minimum_required(VERSION 3.5)

project(Search-engine VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(GTESTS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

include(FetchContent)

if (GTESTS)
	FetchContent_Declare(
	googletest
		URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	)

	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

	FetchContent_MakeAvailable(googletest)
	enable_testing()
endif()

include_directories("include/")
include_directories("ui/")

set(PROJECT_SOURCES
		main.cpp
		include/mainwindow.h
		source/mainwindow.cpp
		ui/ui_main_window.cpp
		ui/ui_request_dialog.cpp
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
		source/exceptions.cpp
		source/engine_core.cpp
		include/comand_line_processor.h
		source/comand_line_processor.cpp
)

add_executable(Search-engine
			${PROJECT_SOURCES}

        )

file(COPY icons DESTINATION ${PROJECT_BINARY_DIR})

if (GTESTS)
	target_link_libraries(Search-engine
		PRIVATE
		Qt5::Widgets
		PUBLIC
		GTest::gtest_main
	)
else()
	target_link_libraries(Search-engine
		PRIVATE
		Qt5::Widgets
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

	target_link_libraries(Search-engine-tests
		PUBLIC
        GTest::gtest_main
        Qt5::Core
	)

	include(GoogleTest)
	gtest_discover_tests(Search-engine-tests)
endif()

