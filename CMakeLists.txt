cmake_minimum_required(VERSION 3.23)

project(ka_common)

include(cmake/ka_common.cmake)

ka_require_cpp_standard(MIN 20 DEFAULT 23)

ka_target(current_target LIBRARY NAME common)

target_sources(${current_target}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/ka/common/assert.hpp
        include/ka/common/cast.hpp
        include/ka/common/fixed.hpp
        include/ka/common/hash.hpp
        include/ka/common/log.hpp
        include/ka/common/pragma.hpp

    PRIVATE
        src/log.cpp
)

find_package(fmt CONFIG REQUIRED)

target_link_libraries(${current_target}
    PRIVATE
        fmt::fmt
)

if(BUILD_TESTING)
    ka_gtest_target(${current_target}_test
        SOURCES
            test/test_assert.cpp
            test/test_cast.cpp
            test/test_hash.cpp
    )

    target_link_libraries(${current_target}_test
        PRIVATE
            ka::common
    )

    foreach(FUNC_SPEC IN ITEMS "consteval" "constexpr")
        foreach(NOEXCEPT_SPEC IN ITEMS "" "noexcept")
            foreach(ASSERT_COND IN ITEMS "true" "false")
                set(TEST_NAME "AssertCompilationSuite_test_assert_${ASSERT_COND}")
                string(APPEND TEST_NAME "_${FUNC_SPEC}")
                if(NOEXCEPT_SPEC)
                    string(APPEND TEST_NAME "_${NOEXCEPT_SPEC}")
                endif()

                add_library(${TEST_NAME} OBJECT ${TEST_SOURCE})
                target_link_libraries(${TEST_NAME} PRIVATE ka::common)
                target_sources(${TEST_NAME} PRIVATE test/test_assert_compile.cpp)
                set_target_properties(${TEST_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
                target_compile_definitions(${TEST_NAME} PRIVATE
                    ASSERT_COND=${ASSERT_COND}
                    FUNC_SPEC=${FUNC_SPEC}
                    NOEXCEPT_SPEC=${NOEXCEPT_SPEC}
                )

                add_test(
                    NAME ${TEST_NAME}
                    COMMAND ${CMAKE_COMMAND} --build . --target ${TEST_NAME} --config $<CONFIGURATION>
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                )
                if (WIN32)
                    # Parallel execution causes problems on Windows.
                    set_tests_properties(${TEST_NAME} PROPERTIES RUN_SERIAL TRUE)
                endif()
                if (ASSERT_COND STREQUAL "false")
                    set_tests_properties(${TEST_NAME} PROPERTIES WILL_FAIL TRUE)
                endif()
            endforeach()
        endforeach()
    endforeach()
endif()
