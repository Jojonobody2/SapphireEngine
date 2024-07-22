#include <iostream>
#include "ImageImporter.h"

#include "Impl/STBImageImporter.h"
#include "Sapphire/Logging/Logger.h"
#include "Sapphire/Import/Image/Impl/WebPImageImporter.h"

namespace Sapphire
{
    BitmapImage ImageImporter::Import(const std::filesystem::path& ImagePath)
    {
        std::string ImageType = ImagePath.extension().string();
        std::transform(ImageType.begin(), ImageType.end(), ImageType.begin(), tolower);

        if (ImageType == ".png" || ImageType == ".jpg" || ImageType == ".jpeg")
        {
            return STBImageImporter::Import(ImagePath);
        }
        else if (ImageType == ".webp")
        {
            return WebPImageImporter::Import(ImagePath);
        }

        SAPPHIRE_ENGINE_ERROR("Image format: {} is not implemented!", ImageType);
        return BitmapImage{};
    }

}
