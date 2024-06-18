cmake_minimum_required(VERSION 3.28)
project(BoidSFML)

set(CMAKE_CXX_STANDARD 20)

add_library(BoidLib STATIC
        src/Creatures/Flock.h
        src/Creatures/Flock.cpp
        src/Behavior/AtomicBehavior.h
        src/Behavior/AtomicBehavior.cpp
        src/Behavior/CombinedBehavior.h
        src/Behavior/CombineBehavior.cpp
        src/utils.h
        src/utils.cpp
        src/World/Noise.h
        src/World/Noise.cpp
        src/World/World.h
        src/World/World.h
        src/Behavior/StatedBehavior.h
        src/Behavior/StatedBehavior.cpp
        src/World/World.cpp
        src/Creatures/Animals.h
        src/Creatures/FlockMember.h
        src/Creatures/FlockMember.h
        src/Behavior/Behavior.h
        src/Creatures/Animals.cpp
        src/World/QuadTree.h
        src/World/QuadTree.h
        src/World/QuadTree.cpp
        src/World/QuadTree.cpp
        src/World/FiniteWorld.h
        src/World/FiniteWorld.h
        src/World/Iterator/NeighborIterator.h
        src/World/Iterator/NeighborIterator.cpp
        src/World/Iterator/NeighborIterator.cpp
        src/World/Iterator/NaiveIterator.h
        src/World/Iterator/NaiveIterator.cpp
        src/World/Iterator/NaiveIterator.cpp
        src/World/Iterator/QuadIterator.h
        src/World/Iterator/QuadIterator.h
        src/World/Iterator/QuadIterator.cpp
        src/World/FiniteWorld.cpp
        src/World/FiniteWorld.cpp
        src/World/FiniteWorld.cpp
)
target_include_directories(BoidLib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(BoidLib PUBLIC  sfml-graphics ImGui-SFML::ImGui-SFML)
add_executable(BoidSFML src/main.cpp

)
target_include_directories(BoidSFML PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

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
add_subdirectory(imgui)

target_link_libraries(BoidSFML sfml-graphics ImGui-SFML::ImGui-SFML BoidLib)

add_custom_target(copy-resources ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources)
file(GLOB RESOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/resources/*.*)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources
        DEPENDS ${CMAKE_SOURCE_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RESOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/resources)
add_dependencies(BoidSFML copy-resources)


# enable_testing() must be in the source directory root (see cmake documentation at https://cmake.org/cmake/help/latest/ command/enable_testing.html)
# Otherwise, Visual Studio test explorer does not see unit tests (See ticket https://developercommunity.visualstudio.com/t/No-tests-discovered-for-googletest-and-C/1148799#T-ND1150621)
enable_testing()

#
# Include GoogleTest
#
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests
        tests/test.cpp

)

target_link_libraries(tests GTest::gtest_main sfml-graphics BoidLib)
target_include_directories(tests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

include(GoogleTest)
gtest_discover_tests(tests)
