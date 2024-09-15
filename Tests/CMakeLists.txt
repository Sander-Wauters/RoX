cmake_minimum_required(VERSION 3.20.0)
project(RoX_Tests LANGUAGES CXX)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(Lib/googletest)

set(SOURCE
   Src/AssetBatchTest.cpp 
)

include_directories(
    Lib/googletest/googletest/include
)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
