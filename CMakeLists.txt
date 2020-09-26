cmake_minimum_required(VERSION 3.15)
project(XPaceTools
        VERSION 1.0
        DESCRIPTION "Parsing library for XPace log-files"
        HOMEPAGE_URL "https://github.com/halirutan/XPaceTools"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_VERSION 1.71)
find_package(Boost ${BOOST_VERSION}
        MODULE
        REQUIRED COMPONENTS unit_test_framework filesystem program_options log)

include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.9.1)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(XPACE_HEADERS
        ${CMAKE_SOURCE_DIR}/src/xpace_math.h
        ${CMAKE_SOURCE_DIR}/src/xpace_log_file.h
        ${CMAKE_SOURCE_DIR}/src/xpace_parser.h
        ${CMAKE_SOURCE_DIR}/src/xpace_statistic.h
        )

set(XPACE_SOURCES
        ${CMAKE_SOURCE_DIR}/src/xpace_log_file.cpp
        ${CMAKE_SOURCE_DIR}/src/xpace_parser.cpp
        ${CMAKE_SOURCE_DIR}/src/xpace_math.cpp
        ${CMAKE_SOURCE_DIR}/src/xpace_statistic.cpp)


# XPace library for loading and working with log-files
add_library(xpaceTools SHARED)
target_sources(xpaceTools
        PRIVATE
        ${XPACE_SOURCES}
        ${XPACE_HEADERS}
        )
target_include_directories(xpaceTools
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        )

# Unit tests
add_executable(xpaceTests)
target_sources(xpaceTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/xpace_log_file_test.cpp)
target_link_libraries(xpaceTests
        PRIVATE
        xpaceTools
        nlohmann_json::nlohmann_json
        ${Boost_LIBRARIES}
        )
target_include_directories(xpaceTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIRS})

add_custom_command(
        TARGET xpaceTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tests/*.log
        ${CMAKE_CURRENT_BINARY_DIR}/)

# Small tools
add_executable(xpaceStatistics)
target_sources(xpaceStatistics
        PRIVATE
        tools/xpaceStatistics.cpp
        )
target_include_directories(xpaceStatistics
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIRS}
)
target_link_libraries(xpaceStatistics
        PRIVATE
        xpaceTools
        nlohmann_json::nlohmann_json
        ${Boost_LIBRARIES}
        )
