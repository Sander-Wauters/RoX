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
add_subdirectory(Tests)

set(SOURCE
    Src/DeviceHandlers/DeviceDataBatch.cpp
    Src/DeviceHandlers/DeviceDataBatch.h
    Src/DeviceHandlers/DeviceResourceData.cpp
    Src/DeviceHandlers/DeviceResourceData.h
    Src/DeviceHandlers/DeviceResources.cpp
    Src/DeviceHandlers/DeviceResources.h
    Src/DeviceHandlers/IDeviceDataSupplier.h
    Src/DeviceHandlers/MaterialDeviceData.cpp
    Src/DeviceHandlers/MaterialDeviceData.h
    Src/DeviceHandlers/MeshDeviceData.cpp
    Src/DeviceHandlers/MeshDeviceData.h
    Src/DeviceHandlers/ModelDeviceData.cpp
    Src/DeviceHandlers/ModelDeviceData.h
    Src/DeviceHandlers/SubmeshDeviceData.cpp
    Src/DeviceHandlers/SubmeshDeviceData.h
    Src/DeviceHandlers/TextDeviceData.cpp
    Src/DeviceHandlers/TextDeviceData.h
    Src/DeviceHandlers/TextureDeviceData.cpp
    Src/DeviceHandlers/TextureDeviceData.h

    Src/Exceptions/DXException.cpp
    Src/Exceptions/ThrowIfFailed.h

    Src/RoX/Animation.cpp
    Src/RoX/AssetBatch.cpp
    Src/RoX/AssetIO.cpp
    Src/RoX/Camera.cpp
    Src/RoX/DirectionalLight.cpp
    Src/RoX/Identifiable.cpp
    Src/RoX/Material.cpp
    Src/RoX/MeshFactory.cpp
    Src/RoX/Model.cpp
    Src/RoX/Outline.cpp
    Src/RoX/Renderer.cpp
    Src/RoX/Scene.cpp
    Src/RoX/Sprite.cpp
    Src/RoX/Timer.cpp
    Src/RoX/Window.cpp

    Src/Util/d3dx12.h
    Src/Util/DebugPrint.h
    Src/Util/dxtk12Pch.h
    Src/Util/Logger.h
    Src/Util/pch.h

    Src/DebugDraw.cpp
    Src/DebugDraw.h
)

set(DEBUGUI_SOURCE
    Src/DebugUI/AssetBatchUI.cpp
    Src/DebugUI/BoneUI.cpp
    Src/DebugUI/CameraUI.cpp
    Src/DebugUI/GeneralUI.cpp
    Src/DebugUI/IdentifiableUI.cpp
    Src/DebugUI/IMeshUI.cpp
    Src/DebugUI/LightingUI.cpp
    Src/DebugUI/MaterialUI.cpp
    Src/DebugUI/MathUI.cpp
    Src/DebugUI/MeshFactoryUI.cpp
    Src/DebugUI/ModelUI.cpp
    Src/DebugUI/OutlineUI.cpp
    Src/DebugUI/SceneUI.cpp
    Src/DebugUI/SpriteUI.cpp
    Src/DebugUI/SubmeshUI.cpp
    Src/DebugUI/TextUI.cpp
    Src/DebugUI/UpdateScheduler.cpp
    Src/DebugUI/Util.cpp
)

include_directories(
    Inc
    Lib/assimp/include
    Lib/DirectXTK12/Inc
    Lib/ImGui/Inc
)

add_library(${PROJECT_NAME} STATIC ${SOURCE} ${DEBUGUI_SOURCE})
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
