project(XpaceToolsLibrary)

set(HEADER_FILES
        xpace_math.h
        xpace_log_file.h
        )

set(SOURCE_FILES xpace_log_file.cpp)

add_library(libXPaceTools STATIC ${SOURCE_FILES} ${HEADER_FILES})
