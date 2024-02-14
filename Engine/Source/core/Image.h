#pragma once

#include <stdint.h>

namespace Sapphire
{
	class Image
	{
	public:
		virtual ~Image();

		static Image Load(const char* Path);

		uint8_t* GetPixels() { return m_Pixels; }
		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetChannels() const { return m_Channels; }

	private:
		Image(uint8_t* Pixels, uint32_t Width, uint32_t Height, uint32_t Channels);

		uint8_t* m_Pixels;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
	};
}