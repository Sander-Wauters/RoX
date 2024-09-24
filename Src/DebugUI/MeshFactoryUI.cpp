#include "DebugUI/MeshFactoryUI.h"

#include <ImGui/imgui.h>

#include "DebugUI/Util.h"
#include "DebugUI/UpdateScheduler.h"

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

void MeshFactoryUI::AddCubeToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddCubeToMesh", &size);
    if (ImGui::Button("Add to mesh##AddCubeToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCube(iMesh, size); });
}

void MeshFactoryUI::AddBoxToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float size[3] = { 1.f, 1.f, 1.f };
    static bool invertNormal = false;

    ImGui::InputFloat3("Size##AddBoxToMesh", size);
    ImGui::Checkbox("Invert normals##AddBoxToMesh", &invertNormal);

    if (ImGui::Button("Add to mesh##AddBoxToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddBox(iMesh, { size[0], size[1], size[2] }, invertNormal); });
}

void MeshFactoryUI::AddSphereToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float diameter = 1.f;
    static std::uint64_t tessellation = 16;
    static bool invertNormal = false;

    ImGui::InputFloat("Diameter##AddSphereToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddSphereToMesh", ImGuiDataType_U64, &tessellation);
    ImGui::Checkbox("Invert normals##AddSphereToMesh", &invertNormal);

    if (ImGui::Button("Add to mesh##AddSphereToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddSphere(iMesh, diameter, tessellation, invertNormal); });
}

void MeshFactoryUI::AddGeoSphereToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float diameter = 1.f;
    static std::uint64_t tessellation = 3;

    ImGui::InputFloat("Diameter##AddGeoSphereToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddGeoSphereToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddGeoSphereToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddGeoSphere(iMesh, diameter, tessellation); });
}

void MeshFactoryUI::AddCylinderToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float height = 1.f;
    static float diameter = 1.f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Height##AddCylinderToMesh", &height);
    ImGui::InputFloat("Diameter##AddCylinderToMesh", &diameter);
    ImGui::InputScalar("Tessellation##AddCylinderToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddCylinderToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCylinder(iMesh, height, diameter, tessellation); });
}

void MeshFactoryUI::AddConeToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float diameter = 1.f;
    static float height = 1.f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Diameter##AddConeToMesh", &diameter);
    ImGui::InputFloat("Height##AddConeToMesh", &height);
    ImGui::InputScalar("Tessellation##AddConeToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddConeToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCone(iMesh, diameter, height, tessellation); });
}

void MeshFactoryUI::AddTorusToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float diameter = 1.f;
    static float thickness = .333f;
    static std::uint64_t tessellation = 32;

    ImGui::InputFloat("Diameter##AddTorusToMesh", &diameter);
    ImGui::InputFloat("Thickness##AddTorusToMesh", &thickness);
    ImGui::InputScalar("Tessellation##AddTorusToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddTorusToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddCone(iMesh, diameter, thickness, tessellation); });
}

void MeshFactoryUI::AddTetrahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddTetrahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddTetrahedronToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddTetrahedron(iMesh, size); });
}

void MeshFactoryUI::AddOctahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddOctahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddOctahedronToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddOctahedron(iMesh, size); });
}

void MeshFactoryUI::AddDodecahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddDodecahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddDodecahedronToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddDodecahedron(iMesh, size); });
}

void MeshFactoryUI::AddIcosahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float size = 1; 

    ImGui::InputFloat("Size##AddIcosahedronToMesh", &size);

    if (ImGui::Button("Add to mesh##AddIcosahedronToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddIcosahedron(iMesh, size); });
}

void MeshFactoryUI::AddTeapotToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static float size = 1; 
    static std::uint64_t tessellation = 8;

    ImGui::InputFloat("Size##AddTeapotToMesh", &size);
    ImGui::InputScalar("Tessellation##AddTeapotToMesh", ImGuiDataType_U64, &tessellation);

    if (ImGui::Button("Add to mesh##AddTeapotToMesh"))
        UpdateScheduler::Get().Add([&](){ MeshFactory::AddTeapot(iMesh, size, tessellation); });
}

void MeshFactoryUI::AddGeoToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh) {
    static MeshFactory::Geometry geo;
    GeoSelector(geo);

    switch (geo) {
        case MeshFactory::Geometry::Cube:
            AddCubeToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Box:
            AddBoxToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Sphere:
            AddSphereToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::GeoSphere:
            AddGeoSphereToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Cylinder:
            AddCylinderToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Cone:
            AddConeToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Torus:
            AddTorusToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Tetrahedron:
            AddTetrahedronToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Octahedron:
            AddOctahedronToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Dodecahedron:
            AddDodecahedronToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Icosahedron:
            AddIcosahedronToIMeshCreator(batch, model, iMesh); break;
        case MeshFactory::Geometry::Teapot:
            AddTeapotToIMeshCreator(batch, model, iMesh); break;
    }
}

