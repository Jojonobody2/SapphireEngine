#pragma once

#include <glm/glm.hpp>

#include <filesystem>

namespace Sapphire
{
    struct Vertex
    {
        alignas(16) glm::vec4 PosUVx;
        alignas(16) glm::vec4 NmlUVy;
    };

    struct MeshData
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;

        uint32_t MaterialIndex;
    };

    struct MaterialData
    {
        std::string DiffuseTexPath;

        uint32_t MaterialIndex;
    };

    struct ModelData
    {
        std::vector<MeshData> MeshDatas;
        std::vector<MaterialData> MaterialDatas;
    };

    class ModelImporter
    {
    public:
        static ModelData Import(const std::filesystem::path& Path);
    };
}