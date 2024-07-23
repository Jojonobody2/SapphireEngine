#pragma once

#include <glm/glm.hpp>

#include <filesystem>

namespace Sapphire
{
    struct Vertex
    {
        alignas(16) glm::vec3 Pos;
        alignas(16) glm::vec3 Nml;
    };

    struct MeshData
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32_t> Indices;
    };

    class ModelImporter
    {
    public:
        static MeshData Import(const std::filesystem::path& Path);
    };
}