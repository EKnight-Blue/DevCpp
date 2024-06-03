cmake_minimum_required(VERSION 3.28)
project(BoidSFML)

set(CMAKE_CXX_STANDARD 20)

add_executable(BoidSFML main.cpp
        Flock.h
        Flock.cpp
        SteeringBehavior.h
        SteeringBehavior.cpp
)

include(FetchContent)

if(WIN32)
    # SonarLint support for CMake requires the following flag to be set on.
    #(see https://community.sonarsource.com/t/sonarlint-for-visual-studio-v4-38-support-cmake-projects/50127)
    set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set (BUILD_SHARED_LIBS FALSE)

if(APPLE)
    find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
    include_directories(${SFML_INCLUDE_DIRS})
else()
    # Linux or Windows
    cmake_policy(SET CMP0135 NEW) # This cmake_policy avoids warning by cmake when we fetch contents based on URL
    FetchContent_Declare(
            sfml
            URL https://github.com/SFML/SFML/archive/refs/tags/2.6.1.tar.gz
    )

    FetchContent_MakeAvailable(sfml)
endif()


target_link_libraries(BoidSFML sfml-graphics)

add_custom_target(copy-resources ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(GLOB RESOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/resources/*.*)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources
        DEPENDS ${CMAKE_SOURCE_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RESOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/resources)
add_dependencies(BoidSFML copy-resources)