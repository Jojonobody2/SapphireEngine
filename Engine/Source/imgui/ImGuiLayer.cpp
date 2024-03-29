#include "ImGuiLayer.h"

#include "ImGuiDebugger.h"

#include "core/Logger.h"

#include "event/KeyEvent.h"
#include "event/MouseEvent.h"

#include "core/Application.h"

#include <imgui.h>

namespace Sapphire
{
	bool Open;

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();

		ImGuiIO& IO = ImGui::GetIO();
		IO.DisplaySize = ImVec2((float)Application::Get().GetWindow()->GetWidth(), (float)Application::Get().GetWindow()->GetWidth());
		IO.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto-Medium.ttf", 20.0f);
		IO.GetClipboardTextFn = ImGuiGetClipboardText;
		IO.SetClipboardTextFn = ImGuiSetClipboardText;
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGui::NewFrame();
	}

	void ImGuiLayer::OnEvent(Event& Event)
	{
		ImGuiIO& IO = ImGui::GetIO();

		switch (Event.GetEventType())
		{
			case EventType::KeyPressed:
			{
				KeyPressedEvent E = (KeyPressedEvent&)Event;
				IO.AddKeyEvent(TranslateKey(E.GetKeyCode()), true);

				if ((E.GetKeyCode() == KeyCode::SK_LEFT_ALT) || (E.GetKeyCode() == KeyCode::SK_RIGHT_ALT))
				{
					IO.AddKeyEvent(ImGuiKey_ModAlt, true);
				}
				else if ((E.GetKeyCode() == KeyCode::SK_LEFT_CONTROL) || (E.GetKeyCode() == KeyCode::SK_RIGHT_CONTROL))
				{
					IO.AddKeyEvent(ImGuiKey_ModCtrl, true);
				}
				else if ((E.GetKeyCode() == KeyCode::SK_LEFT_SUPER) || (E.GetKeyCode() == KeyCode::SK_RIGHT_SUPER))
				{
					IO.AddKeyEvent(ImGuiKey_ModSuper, true);
				}
				else if ((E.GetKeyCode() == KeyCode::SK_LEFT_SHIFT) || (E.GetKeyCode() == KeyCode::SK_RIGHT_SHIFT))
				{
					IO.AddKeyEvent(ImGuiKey_ModShift, true);
				}

				break;
			}

			case EventType::KeyReleased:
			{
				KeyReleasedEvent E = (KeyReleasedEvent&)Event;
				IO.AddKeyEvent(TranslateKey(E.GetKeyCode()), false);

				if ((E.GetKeyCode() == KeyCode::SK_LEFT_ALT) || (E.GetKeyCode() == KeyCode::SK_RIGHT_ALT))
				{
					IO.AddKeyEvent(ImGuiKey_ModAlt, false);
				}
				else if ((E.GetKeyCode() == KeyCode::SK_LEFT_CONTROL) || (E.GetKeyCode() == KeyCode::SK_RIGHT_CONTROL))
				{
					IO.AddKeyEvent(ImGuiKey_ModCtrl, false);
				}	
				else if ((E.GetKeyCode() == KeyCode::SK_LEFT_SUPER) || (E.GetKeyCode() == KeyCode::SK_RIGHT_SUPER))
				{
					IO.AddKeyEvent(ImGuiKey_ModSuper, false);
				}
				else if ((E.GetKeyCode() == KeyCode::SK_LEFT_SHIFT) || (E.GetKeyCode() == KeyCode::SK_RIGHT_SHIFT))
				{
					IO.AddKeyEvent(ImGuiKey_ModShift, false);
				}

				break;
			}

			case EventType::KeyTyped:
			{
				KeyTypedEvent E = (KeyTypedEvent&)Event;
				IO.AddInputCharacter((uint32_t)E.GetTypedChar());

				break;
			}

			case EventType::MouseButtonPressed:
			{
				MouseButtonPressedEvent E = (MouseButtonPressedEvent&)Event;

				//Prevent ImGui from crashing Engine when MouseButton is invalid
				if (E.GetMouseButton() == MouseCode::SM_NONE) return;

				IO.AddMouseButtonEvent(TranslateMouseButton(E.GetMouseButton()), true);

				break;
			}

			case EventType::MouseButtonReleased:
			{
				MouseButtonReleasedEvent E = (MouseButtonReleasedEvent&)Event;

				//Prevent ImGui from crashing Engine when MouseButton is invalid
				if (E.GetMouseButton() == MouseCode::SM_NONE) return;

				IO.AddMouseButtonEvent(TranslateMouseButton(E.GetMouseButton()), false);

				break;
			}

			case EventType::MouseMoved:
			{
				MouseMovedEvent E = (MouseMovedEvent&)Event;
				IO.AddMousePosEvent(E.GetX(), E.GetY());

				break;
			}

			case EventType::MouseScrolled:
			{
				MouseScrolledEvent E = (MouseScrolledEvent&)Event;
				IO.AddMouseWheelEvent(E.GetScrollX(), E.GetScrollY());

				break;
			}
		}
	}

	ImGuiKey ImGuiLayer::TranslateKey(KeyCode Key)
	{
		switch (Key)
		{
			case KeyCode::SK_TAB: 			return ImGuiKey_Tab;
			case KeyCode::SK_LEFT: 			return ImGuiKey_LeftArrow;
			case KeyCode::SK_RIGHT: 		return ImGuiKey_RightArrow;
			case KeyCode::SK_UP: 			return ImGuiKey_UpArrow;
			case KeyCode::SK_DOWN: 			return ImGuiKey_DownArrow;
			case KeyCode::SK_PAGE_UP: 		return ImGuiKey_PageUp;
			case KeyCode::SK_PAGE_DOWN: 	return ImGuiKey_PageDown;
			case KeyCode::SK_HOME: 			return ImGuiKey_Home;
			case KeyCode::SK_END: 			return ImGuiKey_End;
			case KeyCode::SK_INSERT: 		return ImGuiKey_Insert;
			case KeyCode::SK_DELETE: 		return ImGuiKey_Delete;
			case KeyCode::SK_BACKSPACE: 	return ImGuiKey_Backspace;
			case KeyCode::SK_SPACE: 		return ImGuiKey_Space;
			case KeyCode::SK_ENTER: 		return ImGuiKey_Enter;
			case KeyCode::SK_ESCAPE: 		return ImGuiKey_Escape;
			case KeyCode::SK_APOSTROPHE: 	return ImGuiKey_Apostrophe;
			case KeyCode::SK_COMMA: 		return ImGuiKey_Comma;
			case KeyCode::SK_MINUS: 		return ImGuiKey_Minus;
			case KeyCode::SK_PERIOD: 		return ImGuiKey_Period;
			case KeyCode::SK_SLASH: 		return ImGuiKey_Slash;
			case KeyCode::SK_SEMICOLON: 	return ImGuiKey_Semicolon;
			case KeyCode::SK_EQUAL: 		return ImGuiKey_Equal;
			case KeyCode::SK_LEFT_BRACKET: 	return ImGuiKey_LeftBracket;
			case KeyCode::SK_BACKSLASH: 	return ImGuiKey_Backslash;
			case KeyCode::SK_RIGHT_BRACKET: return ImGuiKey_RightBracket;
			case KeyCode::SK_GRAVE_ACCENT: 	return ImGuiKey_GraveAccent;
			case KeyCode::SK_CAPS_LOCK: 	return ImGuiKey_CapsLock;
			case KeyCode::SK_SCROLL_LOCK:	return ImGuiKey_ScrollLock;
			case KeyCode::SK_NUM_LOCK: 		return ImGuiKey_NumLock;
			case KeyCode::SK_PRINT_SCREEN: 	return ImGuiKey_PrintScreen;
			case KeyCode::SK_PAUSE: 		return ImGuiKey_Pause;
			case KeyCode::SK_KP_0: 			return ImGuiKey_Keypad0;
			case KeyCode::SK_KP_1: 			return ImGuiKey_Keypad1;
			case KeyCode::SK_KP_2: 			return ImGuiKey_Keypad2;
			case KeyCode::SK_KP_3: 			return ImGuiKey_Keypad3;
			case KeyCode::SK_KP_4: 			return ImGuiKey_Keypad4;
			case KeyCode::SK_KP_5: 			return ImGuiKey_Keypad5;
			case KeyCode::SK_KP_6: 			return ImGuiKey_Keypad6;
			case KeyCode::SK_KP_7: 			return ImGuiKey_Keypad7;
			case KeyCode::SK_KP_8: 			return ImGuiKey_Keypad8;
			case KeyCode::SK_KP_9: 			return ImGuiKey_Keypad9;
			case KeyCode::SK_KP_DECIMAL:	return ImGuiKey_KeypadDecimal;
			case KeyCode::SK_KP_DIVIDE:		return ImGuiKey_KeypadDivide;
			case KeyCode::SK_KP_MULTIPLY:	return ImGuiKey_KeypadMultiply;
			case KeyCode::SK_KP_SUBTRACT:	return ImGuiKey_KeypadSubtract;
			case KeyCode::SK_KP_ADD:		return ImGuiKey_KeypadAdd;
			case KeyCode::SK_KP_ENTER:		return ImGuiKey_KeyPadEnter;
			case KeyCode::SK_KP_EQUAL:		return ImGuiKey_KeypadEqual;
			case KeyCode::SK_LEFT_SHIFT:	return ImGuiKey_LeftShift;
			case KeyCode::SK_LEFT_CONTROL:	return ImGuiKey_LeftCtrl;
			case KeyCode::SK_LEFT_ALT:		return ImGuiKey_LeftAlt;
			case KeyCode::SK_LEFT_SUPER:	return ImGuiKey_LeftSuper;
			case KeyCode::SK_RIGHT_SHIFT:	return ImGuiKey_RightShift;
			case KeyCode::SK_RIGHT_CONTROL:	return ImGuiKey_RightCtrl;
			case KeyCode::SK_RIGHT_ALT:		return ImGuiKey_RightAlt;
			case KeyCode::SK_RIGHT_SUPER:	return ImGuiKey_RightSuper;
			case KeyCode::SK_MENU: 			return ImGuiKey_Menu;
			case KeyCode::SK_0: 			return ImGuiKey_0;
			case KeyCode::SK_1: 			return ImGuiKey_1;
			case KeyCode::SK_2: 			return ImGuiKey_2;
			case KeyCode::SK_3: 			return ImGuiKey_3;
			case KeyCode::SK_4: 			return ImGuiKey_4;
			case KeyCode::SK_5: 			return ImGuiKey_5;
			case KeyCode::SK_6: 			return ImGuiKey_6;
			case KeyCode::SK_7: 			return ImGuiKey_7;
			case KeyCode::SK_8: 			return ImGuiKey_8;
			case KeyCode::SK_9: 			return ImGuiKey_9;
			case KeyCode::SK_A: 			return ImGuiKey_A;
			case KeyCode::SK_B: 			return ImGuiKey_B;
			case KeyCode::SK_C: 			return ImGuiKey_C;
			case KeyCode::SK_D: 			return ImGuiKey_D;
			case KeyCode::SK_E: 			return ImGuiKey_E;
			case KeyCode::SK_F: 			return ImGuiKey_F;
			case KeyCode::SK_G: 			return ImGuiKey_G;
			case KeyCode::SK_H: 			return ImGuiKey_H;
			case KeyCode::SK_I: 			return ImGuiKey_I;
			case KeyCode::SK_J: 			return ImGuiKey_J;
			case KeyCode::SK_K: 			return ImGuiKey_K;
			case KeyCode::SK_L: 			return ImGuiKey_L;
			case KeyCode::SK_M: 			return ImGuiKey_M;
			case KeyCode::SK_N: 			return ImGuiKey_N;
			case KeyCode::SK_O: 			return ImGuiKey_O;
			case KeyCode::SK_P: 			return ImGuiKey_P;
			case KeyCode::SK_Q: 			return ImGuiKey_Q;
			case KeyCode::SK_R: 			return ImGuiKey_R;
			case KeyCode::SK_S: 			return ImGuiKey_S;
			case KeyCode::SK_T: 			return ImGuiKey_T;
			case KeyCode::SK_U: 			return ImGuiKey_U;
			case KeyCode::SK_V: 			return ImGuiKey_V;
			case KeyCode::SK_W: 			return ImGuiKey_W;
			case KeyCode::SK_X: 			return ImGuiKey_X;
			case KeyCode::SK_Y: 			return ImGuiKey_Y;
			case KeyCode::SK_Z: 			return ImGuiKey_Z;
			case KeyCode::SK_F1: 			return ImGuiKey_F1;
			case KeyCode::SK_F2: 			return ImGuiKey_F2;
			case KeyCode::SK_F3: 			return ImGuiKey_F3;
			case KeyCode::SK_F4: 			return ImGuiKey_F4;
			case KeyCode::SK_F5: 			return ImGuiKey_F5;
			case KeyCode::SK_F6: 			return ImGuiKey_F6;
			case KeyCode::SK_F7: 			return ImGuiKey_F7;
			case KeyCode::SK_F8: 			return ImGuiKey_F8;
			case KeyCode::SK_F9: 			return ImGuiKey_F9;
			case KeyCode::SK_F10: 			return ImGuiKey_F10;
			case KeyCode::SK_F11: 			return ImGuiKey_F11;
			case KeyCode::SK_F12: 			return ImGuiKey_F12;
			case KeyCode::SK_NONE:
			default:						return ImGuiKey_None;
		}
	}

	ImGuiMouseButton ImGuiLayer::TranslateMouseButton(MouseCode MouseButton)
	{
		switch (MouseButton)
		{
			case MouseCode::SM_LEFT:		return ImGuiMouseButton_Left;
			case MouseCode::SM_MIDDLE:		return ImGuiMouseButton_Middle;
			case MouseCode::SM_RIGHT:		return ImGuiMouseButton_Right;
			case MouseCode::SM_NONE:
			default: return -1;
		}
	}

	const char* ImGuiLayer::ImGuiGetClipboardText(void* UserData)
	{
		return Application::Get().GetWindow()->GetClipboardString();
	}

	void ImGuiLayer::ImGuiSetClipboardText(void* UserData, const char* Text)
	{
		Application::Get().GetWindow()->SetClipboardString(Text);
	}
}