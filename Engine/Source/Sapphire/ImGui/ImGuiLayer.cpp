#include "ImGuiLayer.h"

#include "Sapphire/Core/Application.h"
#include "Sapphire/Logging/Logger.h"
#include "Sapphire/Event/MouseEvent.h"
#include "Sapphire/Event/KeyEvent.h"

#include <imgui.h>

namespace Sapphire
{
    void ImGuiLayer::OnAttach()
    {
        ImGui::CreateContext();

        ImGuiIO& IO = ImGui::GetIO();
        IO.Fonts->AddFontFromFileTTF("Resources/Fonts/JetBrains Mono.ttf", 18);
    }

    bool w, a, s, d;
    bool mb, first = true;

    float sens = 0.2f;

    float lx, ly;

    void ImGuiLayer::OnUpdate(double Delta)
    {
        ImGuiIO& IO = ImGui::GetIO();
        IO.DisplaySize = { (float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight() };
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        IO.DeltaTime = (float)Delta / 1000.0f;

        if (d) Application::Get().GetRenderer().GetCamera().Move({ 0.005f * Delta, 0, 0 });
        if (w) Application::Get().GetRenderer().GetCamera().Move({ 0, 0, -0.005f * Delta });
        if (a) Application::Get().GetRenderer().GetCamera().Move({ -0.005f * Delta, 0, 0 });
        if (s) Application::Get().GetRenderer().GetCamera().Move({ 0, 0, 0.005f * Delta });

        ImGui::NewFrame();
    }

    void ImGuiLayer::OnEvent(Event& E)
    {
        ImGuiIO& IO = ImGui::GetIO();

        switch(E.GetEventType())
        {
            case EventType::KeyPressed:
            {
                KeyPressedEvent Event = *reinterpret_cast<KeyPressedEvent*>(&E);
                IO.AddKeyEvent(TranslateKey(Event.GetKeyCode()), true);

                if (Event.GetKeyCode() == KeyCode::SK_W) w = true;
                if (Event.GetKeyCode() == KeyCode::SK_A) a = true;
                if (Event.GetKeyCode() == KeyCode::SK_S) s = true;
                if (Event.GetKeyCode() == KeyCode::SK_D) d = true;

                break;
            }
            case EventType::KeyReleased:
            {
                KeyReleasedEvent Event = *reinterpret_cast<KeyReleasedEvent*>(&E);
                IO.AddKeyEvent(TranslateKey(Event.GetKeyCode()), false);

                if (Event.GetKeyCode() == KeyCode::SK_W) w = false;
                if (Event.GetKeyCode() == KeyCode::SK_A) a = false;
                if (Event.GetKeyCode() == KeyCode::SK_S) s = false;
                if (Event.GetKeyCode() == KeyCode::SK_D) d = false;

                break;
            }
            case EventType::KeyTyped:
            {
                KeyTypedEvent Event = *reinterpret_cast<KeyTypedEvent*>(&E);
                IO.AddInputCharacter(Event.GetTypedChar());

                break;
            }
            case EventType::MouseButtonPressed:
            {
                MouseButtonPressedEvent Event = *reinterpret_cast<MouseButtonPressedEvent*>(&E);
                IO.AddMouseButtonEvent(TranslateMouseButton(Event.GetMouseButton()), true);

                if (Event.GetMouseButton() == MouseCode::SM_LEFT) mb = true;

                break;
            }
            case EventType::MouseButtonReleased:
            {
                MouseButtonReleasedEvent Event = *reinterpret_cast<MouseButtonReleasedEvent*>(&E);
                IO.AddMouseButtonEvent(TranslateMouseButton(Event.GetMouseButton()), false);

                if (Event.GetMouseButton() == MouseCode::SM_LEFT) mb = false;

                break;
            }
            case EventType::MouseMoved:
            {
                MouseMovedEvent Event = *reinterpret_cast<MouseMovedEvent*>(&E);
                IO.AddMousePosEvent(Event.GetX(), Event.GetY());

                if (!first && mb)
                    Application::Get().GetRenderer().GetCamera().Rotate(-(Event.GetY() - ly) * sens, (Event.GetX() - lx) * sens);
                else
                    first = false;

                lx = Event.GetX();
                ly = Event.GetY();

                break;
            }
            case EventType::MouseScrolled:
            {
                MouseScrolledEvent Event = *reinterpret_cast<MouseScrolledEvent*>(&E);
                IO.AddMouseWheelEvent(Event.GetScrollX(), Event.GetScrollY());

                break;
            }

            case EventType::None:
            default:
                break;
        }
    }

    int ImGuiLayer::TranslateMouseButton(MouseCode MouseCode)
    {
        switch (MouseCode)
        {
            case MouseCode::SM_LEFT:
                return 0;
            case MouseCode::SM_MIDDLE:
                return 2;
            case MouseCode::SM_RIGHT:
                return 1;
            case MouseCode::SM_NONE:
            default:
                return -1;
        }
    }

    ImGuiKey ImGuiLayer::TranslateKey(KeyCode KeyCode)
    {
        switch (KeyCode)
        {
            case KeyCode::SK_TAB:
                return ImGuiKey_Tab;
            case KeyCode::SK_LEFT:
                return ImGuiKey_LeftArrow;
            case KeyCode::SK_RIGHT:
                return ImGuiKey_RightArrow;
            case KeyCode::SK_UP:
                return ImGuiKey_UpArrow;
            case KeyCode::SK_DOWN:
                return ImGuiKey_DownArrow;
            case KeyCode::SK_PAGE_UP:
                return ImGuiKey_PageUp;
            case KeyCode::SK_PAGE_DOWN:
                return ImGuiKey_PageDown;
            case KeyCode::SK_HOME:
                return ImGuiKey_Home;
            case KeyCode::SK_END:
                return ImGuiKey_End;
            case KeyCode::SK_INSERT:
                return ImGuiKey_Insert;
            case KeyCode::SK_DELETE:
                return ImGuiKey_Delete;
            case KeyCode::SK_BACKSPACE:
                return ImGuiKey_Backspace;
            case KeyCode::SK_SPACE:
                return ImGuiKey_Space;
            case KeyCode::SK_ENTER:
                return ImGuiKey_Enter;
            case KeyCode::SK_ESCAPE:
                return ImGuiKey_Escape;
            case KeyCode::SK_APOSTROPHE:
                return ImGuiKey_Apostrophe;
            case KeyCode::SK_COMMA:
                return ImGuiKey_Comma;
            case KeyCode::SK_MINUS:
                return ImGuiKey_Minus;
            case KeyCode::SK_PERIOD:
                return ImGuiKey_Period;
            case KeyCode::SK_SLASH:
                return ImGuiKey_Slash;
            case KeyCode::SK_SEMICOLON:
                return ImGuiKey_Semicolon;
            case KeyCode::SK_EQUAL:
                return ImGuiKey_Equal;
            case KeyCode::SK_LEFT_BRACKET:
                return ImGuiKey_LeftBracket;
            case KeyCode::SK_BACKSLASH:
                return ImGuiKey_Backslash;
            case KeyCode::SK_RIGHT_BRACKET:
                return ImGuiKey_RightBracket;
            case KeyCode::SK_GRAVE_ACCENT:
                return ImGuiKey_GraveAccent;
            case KeyCode::SK_CAPS_LOCK:
                return ImGuiKey_CapsLock;
            case KeyCode::SK_SCROLL_LOCK:
                return ImGuiKey_ScrollLock;
            case KeyCode::SK_NUM_LOCK:
                return ImGuiKey_NumLock;
            case KeyCode::SK_PRINT_SCREEN:
                return ImGuiKey_PrintScreen;
            case KeyCode::SK_PAUSE:
                return ImGuiKey_Pause;
            case KeyCode::SK_KP_0:
                return ImGuiKey_Keypad0;
            case KeyCode::SK_KP_1:
                return ImGuiKey_Keypad1;
            case KeyCode::SK_KP_2:
                return ImGuiKey_Keypad2;
            case KeyCode::SK_KP_3:
                return ImGuiKey_Keypad3;
            case KeyCode::SK_KP_4:
                return ImGuiKey_Keypad4;
            case KeyCode::SK_KP_5:
                return ImGuiKey_Keypad5;
            case KeyCode::SK_KP_6:
                return ImGuiKey_Keypad6;
            case KeyCode::SK_KP_7:
                return ImGuiKey_Keypad7;
            case KeyCode::SK_KP_8:
                return ImGuiKey_Keypad8;
            case KeyCode::SK_KP_9:
                return ImGuiKey_Keypad9;
            case KeyCode::SK_KP_DECIMAL:
                return ImGuiKey_KeypadDecimal;
            case KeyCode::SK_KP_DIVIDE:
                return ImGuiKey_KeypadDivide;
            case KeyCode::SK_KP_MULTIPLY:
                return ImGuiKey_KeypadMultiply;
            case KeyCode::SK_KP_SUBTRACT:
                return ImGuiKey_KeypadSubtract;
            case KeyCode::SK_KP_ADD:
                return ImGuiKey_KeypadAdd;
            case KeyCode::SK_KP_ENTER:
                return ImGuiKey_KeypadEnter;
            case KeyCode::SK_KP_EQUAL:
                return ImGuiKey_KeypadEqual;
            case KeyCode::SK_LEFT_SHIFT:
                return ImGuiKey_LeftShift;
            case KeyCode::SK_LEFT_CONTROL:
                return ImGuiKey_LeftCtrl;
            case KeyCode::SK_LEFT_ALT:
                return ImGuiKey_LeftAlt;
            case KeyCode::SK_LEFT_SUPER:
                return ImGuiKey_LeftSuper;
            case KeyCode::SK_RIGHT_SHIFT:
                return ImGuiKey_RightShift;
            case KeyCode::SK_RIGHT_CONTROL:
                return ImGuiKey_RightCtrl;
            case KeyCode::SK_RIGHT_ALT:
                return ImGuiKey_RightAlt;
            case KeyCode::SK_RIGHT_SUPER:
                return ImGuiKey_RightSuper;
            case KeyCode::SK_MENU:
                return ImGuiKey_Menu;
            case KeyCode::SK_0:
                return ImGuiKey_0;
            case KeyCode::SK_1:
                return ImGuiKey_1;
            case KeyCode::SK_2:
                return ImGuiKey_2;
            case KeyCode::SK_3:
                return ImGuiKey_3;
            case KeyCode::SK_4:
                return ImGuiKey_4;
            case KeyCode::SK_5:
                return ImGuiKey_5;
            case KeyCode::SK_6:
                return ImGuiKey_6;
            case KeyCode::SK_7:
                return ImGuiKey_7;
            case KeyCode::SK_8:
                return ImGuiKey_8;
            case KeyCode::SK_9:
                return ImGuiKey_9;
            case KeyCode::SK_A:
                return ImGuiKey_A;
            case KeyCode::SK_B:
                return ImGuiKey_B;
            case KeyCode::SK_C:
                return ImGuiKey_C;
            case KeyCode::SK_D:
                return ImGuiKey_D;
            case KeyCode::SK_E:
                return ImGuiKey_E;
            case KeyCode::SK_F:
                return ImGuiKey_F;
            case KeyCode::SK_G:
                return ImGuiKey_G;
            case KeyCode::SK_H:
                return ImGuiKey_H;
            case KeyCode::SK_I:
                return ImGuiKey_I;
            case KeyCode::SK_J:
                return ImGuiKey_J;
            case KeyCode::SK_K:
                return ImGuiKey_K;
            case KeyCode::SK_L:
                return ImGuiKey_L;
            case KeyCode::SK_M:
                return ImGuiKey_M;
            case KeyCode::SK_N:
                return ImGuiKey_N;
            case KeyCode::SK_O:
                return ImGuiKey_O;
            case KeyCode::SK_P:
                return ImGuiKey_P;
            case KeyCode::SK_Q:
                return ImGuiKey_Q;
            case KeyCode::SK_R:
                return ImGuiKey_R;
            case KeyCode::SK_S:
                return ImGuiKey_S;
            case KeyCode::SK_T:
                return ImGuiKey_T;
            case KeyCode::SK_U:
                return ImGuiKey_U;
            case KeyCode::SK_V:
                return ImGuiKey_V;
            case KeyCode::SK_W:
                return ImGuiKey_W;
            case KeyCode::SK_X:
                return ImGuiKey_X;
            case KeyCode::SK_Y:
                return ImGuiKey_Y;
            case KeyCode::SK_Z:
                return ImGuiKey_Z;
            case KeyCode::SK_F1:
                return ImGuiKey_F1;
            case KeyCode::SK_F2:
                return ImGuiKey_F2;
            case KeyCode::SK_F3:
                return ImGuiKey_F3;
            case KeyCode::SK_F4:
                return ImGuiKey_F4;
            case KeyCode::SK_F5:
                return ImGuiKey_F5;
            case KeyCode::SK_F6:
                return ImGuiKey_F6;
            case KeyCode::SK_F7:
                return ImGuiKey_F7;
            case KeyCode::SK_F8:
                return ImGuiKey_F8;
            case KeyCode::SK_F9:
                return ImGuiKey_F9;
            case KeyCode::SK_F10:
                return ImGuiKey_F10;
            case KeyCode::SK_F11:
                return ImGuiKey_F11;
            case KeyCode::SK_F12:
                return ImGuiKey_F12;
            default:
                return ImGuiKey_None;
        }
    }
}