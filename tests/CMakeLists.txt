set (Boost_USE_STATIC_LIBS OFF)
find_package (Boost REQUIRED COMPONENTS unit_test_framework filesystem)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(Test_run xpace_log_file_test.cpp)
target_link_libraries(Test_run ${Boost_LIBRARIES})
target_link_libraries(Test_run libXPaceTools)

add_custom_command(
        TARGET Test_run POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/tests/xpace_603.log
        ${CMAKE_CURRENT_BINARY_DIR}/xpace_603.log)
