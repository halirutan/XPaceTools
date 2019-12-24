project(XpaceToolsLibrary)
find_package (Boost 1.71 REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

set(HEADER_FILES
        xpace_math.h
        xpace_log_file.h
        xpace_parser.h)

set(SOURCE_FILES xpace_log_file.cpp xpace_parser.cpp xpace_math.cpp)

add_library(libXPaceTools ${SOURCE_FILES} ${HEADER_FILES})
