include(CTest)
# Conan sets BUILD_TESTING=OFF when `tools.build:skip_test` is True.
# CTest normally defines this when using `include(CTest)`.
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    find_package(GTest CONFIG)
    if (GTest_FOUND)
        include(GoogleTest)
    else()
        message(STATUS "GTest package is not found")
    endif()
    enable_testing()
endif()

function(target_enable_warnings TARGET_NAME)
    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
    # https://github.com/fmtlib/fmt/issues/3540
    target_compile_definitions(${TARGET_NAME} PRIVATE _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
endfunction()

function(ka_target current_target_var target_type)
    cmake_parse_arguments(
        PARSE_ARGV 2 arg
        ""
        "NAME"
        ""
    )

    if(NOT DEFINED arg_NAME)
        get_filename_component(component_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    else()
        set(component_name ${arg_NAME})
    endif()
    set(target_name "ka_${component_name}")
    set(alias_name "ka::${component_name}")
    set(${current_target_var} ${target_name} PARENT_SCOPE)

    if(target_type STREQUAL "INTERFACE_LIBRARY")
        add_library(${target_name} INTERFACE)
        add_library(${alias_name} ALIAS ${target_name})
    elseif(target_type STREQUAL "LIBRARY")
        add_library(${target_name})
        add_library(${alias_name} ALIAS ${target_name})

        target_enable_warnings(${target_name})
    elseif(target_type STREQUAL "EXECUTABLE")
        add_executable(${target_name})
        add_executable(${alias_name} ALIAS ${target_name})

        target_enable_warnings(${target_name})
    endif()

    # Add FILE_SET HEADERS explicitly before install command.
    # Without this, header files are not installed on some environments (GitHub Linux Runners).
    target_sources(${target_name} PUBLIC FILE_SET HEADERS BASE_DIRS include)

    install(TARGETS ${target_name}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        FILE_SET HEADERS DESTINATION include
    )
endfunction()

function(ka_gtest_target test_target_name)
    cmake_parse_arguments(
        PARSE_ARGV 1 arg
        ""
        ""
        "SOURCES"
    )

    add_executable(${test_target_name})
    target_sources(${test_target_name} PUBLIC ${arg_SOURCES})
    target_enable_warnings(${test_target_name})
    target_link_libraries(${test_target_name}
        PRIVATE
            gtest::gtest
    )
    gtest_discover_tests(${test_target_name})
endfunction()
