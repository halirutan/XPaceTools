cmake_minimum_required(VERSION 3.15)
project(XPaceTools
        VERSION 1.0
        DESCRIPTION "Parsing library for XPace log-files"
        HOMEPAGE_URL "https://github.com/halirutan/XPaceTools"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_VERSION 1.71)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)
find_package(Boost ${BOOST_VERSION}
        MODULE
        REQUIRED COMPONENTS unit_test_framework filesystem program_options log)

include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.9.1)
FetchContent_MakeAvailable(json)

include(FetchContent)
FetchContent_Declare(ctml
        GIT_REPOSITORY https://github.com/tinfoilboy/CTML.git
        GIT_TAG 1.1.1)
FetchContent_MakeAvailable(ctml)

set(XPACE_HEADERS
        ${CMAKE_SOURCE_DIR}/src/math.hpp
        ${CMAKE_SOURCE_DIR}/src/log_file.hpp
        ${CMAKE_SOURCE_DIR}/src/parser.hpp
        ${CMAKE_SOURCE_DIR}/src/statistic.hpp
        ${CMAKE_SOURCE_DIR}/src/gaussian_filter.hpp
        )

set(XPACE_SOURCES
        ${CMAKE_SOURCE_DIR}/src/log_file.cpp
        ${CMAKE_SOURCE_DIR}/src/parser.cpp
        ${CMAKE_SOURCE_DIR}/src/math.cpp
        ${CMAKE_SOURCE_DIR}/src/statistic.cpp
        ${CMAKE_SOURCE_DIR}/src/gaussian_filter.cpp
        )


# XPace library for loading and working with log-files
add_library(xpaceTools STATIC)
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
        PUBLIC
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
        nlohmann_json::nlohmann_json
        ${Boost_LIBRARIES}
        xpaceTools
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
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
        nlohmann_json::nlohmann_json
        CTML
        ${Boost_LIBRARIES}
        )
