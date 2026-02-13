cmake_minimum_required(VERSION 3.23)

project(ka_common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/ka_common.cmake)

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
            test/test_cast.cpp
            test/test_hash.cpp
    )

    target_link_libraries(${current_target}_test
        PRIVATE
            ka::common
    )
endif()
