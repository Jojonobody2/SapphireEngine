#pragma once

#include <imgui.h>

#include "core/Layer.h"
#include "core/KeyCodes.h"

namespace Sapphire
{
	class ImGuiLayer : public Layer
	{
	public:
		void OnAttach() override;
		void OnUpdate() override;
		void OnEvent(Event& Event);
		void OnDetach() override;

	private:
		ImGuiKey TranslateKey(KeyCode Key);
		ImGuiMouseButton TranslateMouseButton(MouseCode Key);

	private:
		static const char* ImGuiGetClipboardText(void* UserData);
		static void ImGuiSetClipboardText(void* UserData, const char* Text);
	};
}