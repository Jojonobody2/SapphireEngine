#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>

namespace Sapphire
{
    struct BinaryFile
    {
        size_t BlobSize;
        std::vector<uint8_t> BinaryBlob;
    };

    class BinaryFileImporter
    {
    public:
        static BinaryFile Import(const std::filesystem::path& FilePath);
    };
}