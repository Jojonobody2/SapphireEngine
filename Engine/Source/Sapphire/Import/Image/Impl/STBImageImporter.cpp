#include "STBImageImporter.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Sapphire
{
    BitmapImage STBImageImporter::Import(const std::filesystem::path& ImagePath)
    {
        BitmapImage Image{};

        int32_t Width{}, Height{}, Channels{};

        stbi_uc* PixelData = stbi_load(ImagePath.string().c_str(), &Width, &Height, &Channels, STBI_rgb_alpha);

        Image.Width = Width;
        Image.Height = Height;

        Image.Pixels.resize(Width * Height * 4);
        memcpy(Image.Pixels.data(), PixelData, Image.Pixels.size());

        stbi_image_free(PixelData);

        return Image;
    }
}