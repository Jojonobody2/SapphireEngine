#include "Image.h"

#include "Logger.h"

#pragma warning(push, 0)
#include <stb_image.h>
#pragma warning(pop)

namespace Sapphire
{
	Image::Image(uint8_t* Pixels, uint32_t Width, uint32_t Height, uint32_t Channels)
		: m_Pixels(Pixels), m_Width(Width), m_Height(Height), m_Channels(Channels)
	{}

	Image::~Image()
	{
		stbi_image_free(m_Pixels);
	}

	Image Image::Load(const char* Path)
	{
		int Width{}, Height{}, Channels{};

		uint8_t* Pixels = stbi_load(Path, &Width, &Height, &Channels, STBI_rgb_alpha);

		if (Pixels == nullptr)
			SAPPHIRE_ERROR("Failed to load image: {}\n{}!", Path, stbi_failure_reason());

		return Image(Pixels, Width, Height, Channels);
	}
}
