cmake_minimum_required(VERSION 3.20.0)
project(RoX 
    VERSION 0.0.0
    LANGUAGES CXX
)

set(SOURCE
    Src/Exceptions/DXException.cpp
    Src/Exceptions/ThrowIfFailed.h

    Src/Util/d3dx12.h
    Src/Util/Logger.h
    Src/Util/pch.h

    Src/DebugDraw.cpp
    Src/DebugDraw.h
    Src/DeviceResources.cpp
    Src/DeviceResources.h
    Src/IDeviceNotify.h
    Src/Renderer.cpp
    Src/SpriteData.h
    Src/TextData.h
    Src/Timer.cpp
    Src/Window.cpp
)

include_directories(Inc)
add_library(${PROJECT_NAME} STATIC ${SOURCE})
target_link_libraries(${PROJECT_NAME} PRIVATE 
    d3d12.lib dxgi.lib dxguid.lib uuid.lib
    kernel32.lib user32.lib
    comdlg32.lib advapi32.lib shell32.lib
    ole32.lib oleaut32.lib
    runtimeobject.lib
    DirectXTK12
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
