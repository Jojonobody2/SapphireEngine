#include "ImageImporter.h"

#include "Impl/STBImageImporter.h"
#include "Sapphire/Logging/Logger.h"

namespace Sapphire
{
    Image ImageImporter::Import(const std::filesystem::path& ImagePath)
    {
        std::filesystem::path ImageType = ImagePath.extension();

        if (ImageType == ".png" || ImageType == ".jpg" || ImageType == ".jpeg")
        {
            return STBImageImporter::Import(ImagePath);
        }

        SAPPHIRE_ENGINE_ERROR("Image format: {} is not implemented!", ImageType.string());
        return Image{};
    }

}
