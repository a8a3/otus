cmake_minimum_required(VERSION 3.2)

if(DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(ip_filter VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(ip_filter VERSION 0.0.1)
endif()

find_package(Threads REQUIRED)
find_package(GTest   REQUIRED)

add_executable(ip_filter        sources/ip_filter.cpp)
add_executable(ip_filter_gtests tests/ip_filter_gtests.cpp)

set_target_properties(ip_filter PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(ip_filter_gtests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/sources
)

target_link_libraries(ip_filter_gtests 
    ${GTEST_BOTH_LIBRARIES}
    pthread
)

if (MSVC)
    target_compile_options(ip_filter PRIVATE
        /W4
    )
    target_compile_options(ip_filter_gtests PRIVATE
        /W4
    )
else ()
    target_compile_options(ip_filter PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(ip_filter_gtests PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

install(TARGETS ip_filter RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT alexpoligon@mail.ru)

include(CPack)

enable_testing()

add_test(ip_filter_gtests ip_filter_gtests)

add_custom_command(
    TARGET ip_filter_gtests
    POST_BUILD 
    COMMAND ./ip_filter_gtests
    COMMENT "Run ip_filter_gtests tests..."
)