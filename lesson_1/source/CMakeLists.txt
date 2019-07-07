cmake_minimum_required(VERSION 3.2)

if(DEFINED ENV{TRAVIS_BUILD_NUMBER})
    message("Build on Travis")
    project(helloworld VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    message("Build on localhost")
    project(helloworld VERSION 0.0.0)
endif()

find_package(Boost   COMPONENTS unit_test_framework REQUIRED)
find_package(Threads REQUIRED)
find_package(GTest   REQUIRED)

configure_file(version.h.in version.h)

add_executable(helloworld_cli main.cpp)
add_library(helloworld lib.cpp)
add_executable(test_version test_version.cpp)
add_executable(gtest_version gtest_version.cpp)

set_target_properties(helloworld_cli helloworld test_version gtest_version PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(helloworld
    PRIVATE "${CMAKE_BINARY_DIR}"
)
set_target_properties(test_version PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld
)

target_link_libraries(test_version
    ${Boost_LIBRARIES}
    helloworld
)

target_link_libraries(gtest_version
    gtest
    gtest_main
    helloworld
    pthread
)

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE
        /W4
    )
    target_compile_options(helloworld PRIVATE
        /W4
    )
    target_compile_options(test_version PRIVATE
        /W4
    )
    target_compile_options(gtest_version PRIVATE
        /W4
    )
else ()
    target_compile_options(helloworld_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(helloworld PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(gtest_version PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()



install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT alexpoligon@mail.ru)

include(CPack)

enable_testing()

add_test(test_version test_version)
add_test(gtest_version gtest_version)

add_custom_command(
    TARGET gtest_version
    POST_BUILD 
    COMMAND ./gtest_version
    COMMENT "Run google tests..."
)

add_custom_command(
    TARGET test_version
    POST_BUILD 
    COMMAND ./test_version
    COMMENT "Run boost tests..."
)