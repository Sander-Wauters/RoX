cmake_minimum_required(VERSION 3.20.0)
project(RoX 
    VERSION 0.0.0
    LANGUAGES CXX
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Assimp build tests" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "Debug postfix" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "Assimp install" FORCE)

add_subdirectory(Lib/assimp)
add_subdirectory(Lib/DirectXTK12)
add_subdirectory(Lib/ImGui)

set(SOURCE
    Src/Exceptions/DXException.cpp
    Src/Exceptions/ThrowIfFailed.h

    Src/Util/d3dx12.h
    Src/Util/Logger.h
    Src/Util/pch.h

    Src/AssetIO.cpp
    Src/Camera.cpp
    Src/DebugDraw.cpp
    Src/DebugDraw.h
    #Src/DeviceData.cpp
    #Src/DeviceData.h
    Src/DeviceResourceData.cpp
    Src/DeviceResourceData.h
    Src/DeviceResources.cpp
    Src/DeviceResources.h
    Src/IDeviceObserver.h
    Src/Material.cpp
    Src/MeshDeviceData.cpp
    Src/MeshDeviceData.h
    Src/Model.cpp
    Src/ModelDeviceData.cpp
    Src/ModelDeviceData.h
    Src/Renderer.cpp
    Src/Scene.cpp
    Src/SubmeshDeviceData.cpp
    Src/SubmeshDeviceData.h
    Src/SubmeshFactory.cpp
    Src/TextDeviceData.h
    Src/TextureDeviceData.h
    Src/Timer.cpp
    Src/Window.cpp
)

include_directories(
    Inc 
    Lib/assimp/include
    Lib/DirectXTK12/Inc
    Lib/ImGui/Inc
)

add_library(${PROJECT_NAME} STATIC ${SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    d3d12.lib dxgi.lib dxguid.lib uuid.lib
    kernel32.lib user32.lib
    comdlg32.lib advapi32.lib shell32.lib
    ole32.lib oleaut32.lib
    runtimeobject.lib
    DirectXTK12
    assimp
    ImGui
)

#--------------- Build HLSL shaders ---------------#
#add_custom_target(shaders)
#
#set(HLSL_SHADER_FILES 
#    Shaders/VertexShader.hlsl 
#    Shaders/PixelShader.hlsl
#)
#
#set_source_files_properties(Shaders/VertexShader.hlsl PROPERTIES ShaderType "vs")
#set_source_files_properties(Shaders/PixelShader.hlsl PROPERTIES ShaderType "ps")
#set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES ShaderModel "5_0")
#
#foreach(FILE ${HLSL_SHADER_FILES})
#    get_filename_component(FILE_WE ${FILE} NAME_WE)
#    get_source_file_property(shadertype ${FILE} ShaderType)
#    get_source_file_property(shadermodel ${FILE} ShaderModel)
#    add_custom_command(
#        TARGET shaders
#        COMMAND fxc.exe /E main /T${shadertype}_${shadermodel} /Zi /Fo ${CMAKE_BINARY_DIR}/demoApp/Debug/bin/shaders/${FILE_WE}.cso ${FILE}
#        MAIN_DEPENDENCY ${FILE}
#        COMMENT "HLSL ${FILE}"
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#        VERBATIM
#    )
#endforeach(FILE)
#
#add_dependencies(${PROJECT_NAME} shaders)
