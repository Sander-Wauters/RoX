#pragma once

#include <memory>

#include "../Lib/DirectXTK12/Inc/GeometricPrimitive.h"

struct StaticGeometryData {
    std::unique_ptr<DirectX::GeometricPrimitive> GeometricPrimitive;     
};
