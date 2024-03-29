#pragma once

#include "Image.h"
#include "event/SapphireEvent.h"

#include <memory>

namespace Sapphire
{
	typedef void (*SapphireEventCallback)(Event& Event);

	struct WindowCreateInfo
	{
		const char* Title;
		uint32_t Width;
		uint32_t Height;
	};
	
	class Window
	{
	public:
		virtual ~Window() = default;
		
		virtual void Update() = 0;
		
		virtual void Close() = 0;
		virtual bool IsOpen() = 0;
		virtual bool IsMinimized() = 0;
		
		virtual void SetTitle(const char* Title) = 0;
		virtual void SetIcon(Image Icon, Image IconSm) = 0;

		virtual const char* GetClipboardString() = 0;
		virtual void SetClipboardString(const char* String) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		
		virtual void SetEventCallback(SapphireEventCallback EventCallbackFn) = 0;

		virtual void* GetNativeWindow() = 0;
		
		static std::shared_ptr<Window> Create(const WindowCreateInfo& WindowCI);

	protected:
		void OnClose();
	};
}