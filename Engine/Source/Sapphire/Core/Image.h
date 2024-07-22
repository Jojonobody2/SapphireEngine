#pragma once

#include <cstdint>
#include <vector>

namespace Sapphire
{
    class BitmapImage
    {
    public:
        uint32_t Width, Height;
        std::vector<uint8_t> Pixels;
    };
}
