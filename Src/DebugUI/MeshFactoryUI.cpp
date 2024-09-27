#include "DebugUI/MeshFactoryUI.h"

#include <stdexcept>

#include <ImGui/imgui.h>

#include "DebugUI/Util.h"
#include "DebugUI/UpdateScheduler.h"
#include "DebugUI/GeneralUI.h"

void MeshFactoryUI::GeoSelector(MeshFactory::Geometry& geo) {
    static std::uint8_t selected = 0;
    static const MeshFactory::Geometry options[12] = { MeshFactory::Geometry::Cube, MeshFactory::Geometry::Box, MeshFactory::Geometry::Sphere, MeshFactory::Geometry::GeoSphere, MeshFactory::Geometry::Cylinder, MeshFactory::Geometry::Cone, MeshFactory::Geometry::Torus, MeshFactory::Geometry::Tetrahedron, MeshFactory::Geometry::Octahedron, MeshFactory::Geometry::Dodecahedron, MeshFactory::Geometry::Icosahedron, MeshFactory::Geometry::Teapot };
    static const std::string optionsStr[12] = { "Cube", "Box", "Sphere", "GeoSphere", "Cylinder", "Cone", "Torus", "Tetrahedron", "Octahedron", "Dodecahedron", "Icosahedron", "Teapot" };

    if (ImGui::Button(Util::GUIDLabel(optionsStr[selected], "AddGeoToIMeshCreator").c_str()))
        ImGui::OpenPopup("GeoTypeSelector");
    if (ImGui::BeginPopup("GeoTypeSelector")) {
        for (std::uint8_t i = 0; i < std::size(optionsStr); ++i) {
            if (ImGui::Selectable(Util::GUIDLabel(optionsStr[i], "AddGeoToIMeshCreator").c_str(), selected == i)) {
                selected = i;
                geo = options[i];
            }
        }
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    ImGui::Text("Geometry");
}

void MeshFactoryUI::AddCubeToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddCubeToMesh", &size);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddCubeToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCube(iMesh, size); });
}

void MeshFactoryUI::AddBoxToIMeshCreator(IMesh& iMesh) {
    static float size[3] = { 1.f, 1.f, 1.f };
    static bool invertNormal = false;

    ImGui::InputFloat3("Size##AddBoxToMesh", size);
    ImGui::Checkbox("Invert normals##AddBoxToMesh", &invertNormal);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddBoxToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddBox(iMesh, { size[0], size[1], size[2] }, invertNormal); });
}

void MeshFactoryUI::AddSphereToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static std::uint64_t tessellation = 16;
    static bool invertNormal = false;

    ImGui::InputFloat("Diameter##AddSphereToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddSphereToMesh", ImGuiDataType_U64, &tessellation);
    ImGui::Checkbox("Invert normals##AddSphereToMesh", &invertNormal);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddSphereToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddSphere(iMesh, diameter, tessellation, invertNormal); });
}

void MeshFactoryUI::AddGeoSphereToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static std::uint64_t tessellation = 3;

    ImGui::InputFloat("Diameter##AddGeoSphereToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddGeoSphereToMesh", ImGuiDataType_U64, &tessellation);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddGeoSphereToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddGeoSphere(iMesh, diameter, tessellation); });
}

void MeshFactoryUI::AddCylinderToIMeshCreator(IMesh& iMesh) {
    static float height = 1.f;
    static float diameter = 1.f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Height##AddCylinderToMesh", &height);
    ImGui::InputFloat("Diameter##AddCylinderToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddCylinderToMesh", ImGuiDataType_U64, &tessellation);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddCylinderToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCylinder(iMesh, height, diameter, tessellation); });
}

void MeshFactoryUI::AddConeToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static float height = 1.f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Diameter##AddConeToMesh", &diameter);
    ImGui::InputFloat("Height##AddConeToMesh", &height);
    ImGui::InputScalar("Tessellation##AddConeToMesh", ImGuiDataType_U64, &tessellation);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddConeToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCone(iMesh, diameter, height, tessellation); });
}

void MeshFactoryUI::AddTorusToIMeshCreator(IMesh& iMesh) {
    static float diameter = 1.f;
    static float thickness = .333f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Diameter##AddTorusToMesh", &diameter);
    ImGui::InputFloat("Thickness##AddTorusToMesh", &thickness);
    ImGui::InputScalar("Tessellation##AddTorusToMesh", ImGuiDataType_U64, &tessellation);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddTorusToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCone(iMesh, diameter, thickness, tessellation); });
}

void MeshFactoryUI::AddTetrahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddTetrahedronToMesh", &size);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddTetrahedronToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddTetrahedron(iMesh, size); });
}

void MeshFactoryUI::AddOctahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddOctahedronToMesh", &size);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddOctahedronToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddOctahedron(iMesh, size); });
}

void MeshFactoryUI::AddDodecahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    ImGui::InputFloat("Size##AddDodecahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddDodecahedronToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddDodecahedron(iMesh, size); });
}

void MeshFactoryUI::AddIcosahedronToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddIcosahedronToMesh", &size);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddIcosahedronToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddIcosahedron(iMesh, size); });
}

void MeshFactoryUI::AddTeapotToIMeshCreator(IMesh& iMesh) {
    static float size = 1; 
    static std::uint64_t tessellation = 8;

    ImGui::InputFloat("Size##AddTeapotToMesh", &size);
    ImGui::InputScalar("Tessellation##AddTeapotToMesh", ImGuiDataType_U64, &tessellation);

    GeneralUI::Error(iMesh.IsUsingStaticBuffers(), "Can't add goemetry to a mesh with static buffers.");
    if (ImGui::Button("Add to mesh##AddTeapotToMesh") && !iMesh.IsUsingStaticBuffers())
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddTeapot(iMesh, size, tessellation); });
}

void MeshFactoryUI::AddGeoToIMeshCreator(IMesh& iMesh) {
    static MeshFactory::Geometry geo;
    GeoSelector(geo);

    switch (geo) {
        case MeshFactory::Geometry::Cube:
            AddCubeToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Box:
            AddBoxToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Sphere:
            AddSphereToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::GeoSphere:
            AddGeoSphereToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Cylinder:
            AddCylinderToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Cone:
            AddConeToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Torus:
            AddTorusToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Tetrahedron:
            AddTetrahedronToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Octahedron:
            AddOctahedronToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Dodecahedron:
            AddDodecahedronToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Icosahedron:
            AddIcosahedronToIMeshCreator(iMesh); break;
        case MeshFactory::Geometry::Teapot:
            AddTeapotToIMeshCreator(iMesh); break;
    }
}

