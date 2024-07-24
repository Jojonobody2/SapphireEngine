#include "ModelImporter.h"

#include "Impl/AssimpModelImporter.h"

namespace Sapphire
{
    ModelData ModelImporter::Import(const std::filesystem::path& Path)
    {
        return AssimpModelImporter::Import(Path);
    }
}