#include "BinaryFileImporter.h"

#include "Sapphire/Logging/Logger.h"

#include <fstream>

namespace Sapphire
{
    BinaryFile BinaryFileImporter::Import(const std::filesystem::path& FilePath)
    {
        BinaryFile File{};

        std::ifstream FileHandle(FilePath, std::ios::in | std::ios::ate | std::ios::binary);
        if (!FileHandle.is_open())
        {
            SAPPHIRE_ENGINE_ERROR("Failed to import file: {}", FilePath.string());
        }

        File.BlobSize = (size_t)FileHandle.tellg();

        FileHandle.seekg(0);

        File.BinaryBlob.resize(File.BlobSize);
        FileHandle.read((char*)File.BinaryBlob.data(), (std::streamsize)File.BlobSize);

        FileHandle.close();

        return File;
    }
}