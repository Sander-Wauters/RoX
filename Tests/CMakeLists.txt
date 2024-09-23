cmake_minimum_required(VERSION 3.20.0)
project(RoX_Tests LANGUAGES CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(Lib/googletest)

set(TESTS
    Src/PredefinedObjects/ValidAssetBatch.cpp
    Src/PredefinedObjects/ValidAssetBatch.h
    Src/PredefinedObjects/ValidMaterial.cpp
    Src/PredefinedObjects/ValidMaterial.h
    Src/PredefinedObjects/ValidMesh.cpp
    Src/PredefinedObjects/ValidMesh.h
    Src/PredefinedObjects/ValidModel.cpp
    Src/PredefinedObjects/ValidModel.h

    Src/AssetBatchTest.cpp 
    Src/MeshTest.cpp
    Src/MockAssetBatchObserver.h
    Src/MockMeshObserver.h
    Src/MockModelObserver.h
    Src/ModelTest.cpp
    Src/RendererTest.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/RoX/Inc 

    Lib/googletest/googlemock/include
    Lib/googletest/googletest/include
)

add_executable(${PROJECT_NAME} ${TESTS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    gtest_main
    gmock_main
    RoX
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
