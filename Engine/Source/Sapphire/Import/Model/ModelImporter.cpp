#include "ModelImporter.h"

#include "Impl/AssimpModelImporter.h"

namespace Sapphire
{
    MeshData ModelImporter::Import(const std::filesystem::path& Path)
    {
        return AssimpModelImporter::Import(Path);
    }
}