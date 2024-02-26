#pragma once

#include <string>

struct Texture {
    Texture(std::wstring textureFilePath = L"", 
            std::wstring normalMapFilePath = L"assets/smoothMap.dds", 
            std::wstring specularMapFilePath = L"") : 
        TextureFilePath(textureFilePath),
        NormalMapFilePath(normalMapFilePath),
        SpecularMapFilePath(specularMapFilePath) {}

    std::wstring TextureFilePath = L"";
    std::wstring NormalMapFilePath = L"";
    std::wstring SpecularMapFilePath = L"";
};
