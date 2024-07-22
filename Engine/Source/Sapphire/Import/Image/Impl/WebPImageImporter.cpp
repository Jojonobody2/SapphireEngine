#include "WebPImageImporter.h"

#include "Sapphire/Logging/Logger.h"
#include "Sapphire/Import/File/BinaryFileImporter.h"

#include <webp/decode.h>

namespace Sapphire
{
    BitmapImage WebPImageImporter::Import(const std::filesystem::path& ImagePath)
    {
        BitmapImage Image{};

        BinaryFile File = BinaryFileImporter::Import(ImagePath);

        int32_t Width{}, Height{};
        if (!WebPGetInfo(File.BinaryBlob.data(), File.BlobSize, &Width, &Height))
        {
            SAPPHIRE_ENGINE_ERROR("Failed to load WebP Image: {}", ImagePath.string());

            return Image;
        }

        uint8_t* Pixels = WebPDecodeRGBA(File.BinaryBlob.data(), File.BlobSize, &Width, &Height);

        Image.Width = Width;
        Image.Height = Height;

        //Sapphire Images are always in format RGBA so *4
        Image.Pixels.resize(Width * Height * 4);

        memcpy(Image.Pixels.data(), Pixels, Width * Height * 4);

        WebPFree(Pixels);

        return Image;
    }
}