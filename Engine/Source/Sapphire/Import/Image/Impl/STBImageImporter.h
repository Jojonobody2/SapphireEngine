#pragma once

#include "Sapphire/Core/Image.h"

#include <filesystem>

namespace Sapphire
{
    class STBImageImporter
    {
    public:
        static Image Import(const std::filesystem::path& ImagePath);
    };
}
