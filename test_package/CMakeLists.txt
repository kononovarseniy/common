cmake_minimum_required(VERSION 3.23)

project(example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ka_common CONFIG REQUIRED)

include(ka_common)

ka_target(current_target EXECUTABLE NAME example)

target_sources(${current_target}
    PRIVATE
        src/example.cpp
)

target_link_libraries(${current_target}
    PRIVATE
        ka::common
)
