#pragma once

#include "Sapphire/Core/ILayer.h"
#include "Sapphire/Core/InputCodes.h"

#include <imgui.h>

namespace Sapphire
{
    class ImGuiLayer : public ILayer
    {
    private:
        void OnAttach() override;
        void OnUpdate(double Delta) override;
        void OnEvent(Event& E) override;

        static int TranslateMouseButton(MouseCode MouseCode);
        static ImGuiKey TranslateKey(KeyCode KeyCode);
    };
}