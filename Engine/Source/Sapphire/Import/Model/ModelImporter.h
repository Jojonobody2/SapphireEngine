#pragma once

#include <glm/glm.hpp>

#include <filesystem>

namespace Sapphire
{
    struct Vertex
    {
        alignas(16) glm::vec4 PosUVx;
        alignas(16) glm::vec4 NmlUVy;
        alignas(16) glm::vec3 Tan;
        alignas(16) glm::vec3 Bitan;
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
        std::string NormalTexPath;
        std::string PBRTexPath;

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