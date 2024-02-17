#include "GLFWWindow.h"

#include "event/KeyEvent.h"
#include "event/MouseEvent.h"

namespace Sapphire
{
	GlfwWindow::GlfwWindow(const WindowCreateInfo& WindowCI)
	{
		glfwInit();
		
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		m_Window = glfwCreateWindow(WindowCI.Width, WindowCI.Height, WindowCI.Title, nullptr, nullptr);
		
		glfwSetWindowUserPointer(m_Window, this);

		const GLFWvidmode* VidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(m_Window, (VidMode->width - WindowCI.Width) / 2, (VidMode->height - WindowCI.Height) / 2);

		glfwSetKeyCallback(m_Window, KeyCallback);
		glfwSetCharCallback(m_Window, CharCallback);
		glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
		glfwSetScrollCallback(m_Window, MouseScrollCallback);
		glfwSetCursorPosCallback(m_Window, MouseMovedCallback);
		glfwSetWindowCloseCallback(m_Window, WindowCloseCallback);
	}
	
	GlfwWindow::~GlfwWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
	
	void GlfwWindow::Update()
	{
		glfwPollEvents();
	}
	
	void GlfwWindow::Close()
	{
		glfwSetWindowShouldClose(m_Window, true);
	}
	
	bool GlfwWindow::IsOpen()
	{
		return !glfwWindowShouldClose(m_Window);
	}

	bool GlfwWindow::IsMinimized()
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
	}

	void GlfwWindow::SetTitle(const char* Title)
	{
		glfwSetWindowTitle(m_Window, Title);
	}

	void GlfwWindow::SetIcon(Image Icon, Image IconSm)
	{
		GLFWimage IconImages[2];
		IconImages[0].width = Icon.GetWidth();
		IconImages[0].height = Icon.GetHeight();
		IconImages[0].pixels = Icon.GetPixels();

		IconImages[1].width = IconSm.GetWidth();
		IconImages[1].height = IconSm.GetHeight();
		IconImages[1].pixels = IconSm.GetPixels();

		glfwSetWindowIcon(m_Window, 2, IconImages);
	}

	const char* GlfwWindow::GetClipboardString()
	{
		return glfwGetClipboardString(m_Window);
	}

	void GlfwWindow::SetClipboardString(const char* String)
	{
		glfwSetClipboardString(m_Window, String);
	}
	
	uint32_t GlfwWindow::GetWidth() const
	{
		int32_t pWidth;
		glfwGetWindowSize(m_Window, &pWidth, nullptr);
		
		return pWidth;
	}
	
	uint32_t GlfwWindow::GetHeight() const
	{
		int32_t pHeight;
		glfwGetWindowSize(m_Window, nullptr, &pHeight);
		
		return pHeight;
	}

	KeyCode GlfwWindow::TranslateKey(int GlfwKeyCode)
	{
		switch (GlfwKeyCode)
		{
			case GLFW_KEY_TAB:				return KeyCode::SK_TAB;
			case GLFW_KEY_LEFT:				return KeyCode::SK_LEFT;
			case GLFW_KEY_RIGHT:			return KeyCode::SK_RIGHT;
			case GLFW_KEY_UP:				return KeyCode::SK_UP;
			case GLFW_KEY_DOWN:				return KeyCode::SK_DOWN;
			case GLFW_KEY_PAGE_UP:			return KeyCode::SK_PAGE_UP;
			case GLFW_KEY_PAGE_DOWN:		return KeyCode::SK_PAGE_DOWN;
			case GLFW_KEY_HOME:				return KeyCode::SK_HOME;
			case GLFW_KEY_END:				return KeyCode::SK_END;
			case GLFW_KEY_INSERT:			return KeyCode::SK_INSERT;
			case GLFW_KEY_DELETE:			return KeyCode::SK_DELETE;
			case GLFW_KEY_BACKSPACE:		return KeyCode::SK_BACKSPACE;
			case GLFW_KEY_SPACE:			return KeyCode::SK_SPACE;
			case GLFW_KEY_ENTER:			return KeyCode::SK_ENTER;
			case GLFW_KEY_ESCAPE:			return KeyCode::SK_ESCAPE;
			case GLFW_KEY_APOSTROPHE:		return KeyCode::SK_APOSTROPHE;
			case GLFW_KEY_COMMA:			return KeyCode::SK_COMMA;
			case GLFW_KEY_MINUS:			return KeyCode::SK_MINUS;
			case GLFW_KEY_PERIOD:			return KeyCode::SK_PERIOD;
			case GLFW_KEY_SLASH:			return KeyCode::SK_SLASH;
			case GLFW_KEY_SEMICOLON:		return KeyCode::SK_SEMICOLON;
			case GLFW_KEY_EQUAL:			return KeyCode::SK_EQUAL;
			case GLFW_KEY_LEFT_BRACKET:		return KeyCode::SK_LEFT_BRACKET;
			case GLFW_KEY_BACKSLASH:		return KeyCode::SK_BACKSLASH;
			case GLFW_KEY_RIGHT_BRACKET:	return KeyCode::SK_RIGHT_BRACKET;
			case GLFW_KEY_GRAVE_ACCENT:		return KeyCode::SK_GRAVE_ACCENT;
			case GLFW_KEY_CAPS_LOCK:		return KeyCode::SK_CAPS_LOCK;
			case GLFW_KEY_SCROLL_LOCK:		return KeyCode::SK_SCROLL_LOCK;
			case GLFW_KEY_NUM_LOCK:			return KeyCode::SK_NUM_LOCK;
			case GLFW_KEY_PRINT_SCREEN:		return KeyCode::SK_PRINT_SCREEN;
			case GLFW_KEY_PAUSE:			return KeyCode::SK_PAUSE;
			case GLFW_KEY_KP_0:				return KeyCode::SK_KP_0;
			case GLFW_KEY_KP_1:				return KeyCode::SK_KP_1;
			case GLFW_KEY_KP_2:				return KeyCode::SK_KP_2;
			case GLFW_KEY_KP_3:				return KeyCode::SK_KP_3;
			case GLFW_KEY_KP_4:				return KeyCode::SK_KP_4;
			case GLFW_KEY_KP_5:				return KeyCode::SK_KP_5;
			case GLFW_KEY_KP_6:				return KeyCode::SK_KP_6;
			case GLFW_KEY_KP_7:				return KeyCode::SK_KP_7;
			case GLFW_KEY_KP_8:				return KeyCode::SK_KP_8;
			case GLFW_KEY_KP_9:				return KeyCode::SK_KP_9;
			case GLFW_KEY_KP_DECIMAL:		return KeyCode::SK_KP_DECIMAL;
			case GLFW_KEY_KP_DIVIDE:		return KeyCode::SK_KP_DIVIDE;
			case GLFW_KEY_KP_MULTIPLY:		return KeyCode::SK_KP_MULTIPLY;
			case GLFW_KEY_KP_SUBTRACT:		return KeyCode::SK_KP_SUBTRACT;
			case GLFW_KEY_KP_ADD:			return KeyCode::SK_KP_ADD;
			case GLFW_KEY_KP_ENTER:			return KeyCode::SK_KP_ENTER;
			case GLFW_KEY_KP_EQUAL:			return KeyCode::SK_KP_EQUAL;
			case GLFW_KEY_LEFT_SHIFT:		return KeyCode::SK_LEFT_SHIFT;
			case GLFW_KEY_LEFT_CONTROL:		return KeyCode::SK_LEFT_CONTROL;
			case GLFW_KEY_LEFT_ALT:			return KeyCode::SK_LEFT_ALT;
			case GLFW_KEY_LEFT_SUPER:		return KeyCode::SK_LEFT_SUPER;
			case GLFW_KEY_RIGHT_SHIFT:		return KeyCode::SK_RIGHT_SHIFT;
			case GLFW_KEY_RIGHT_CONTROL:	return KeyCode::SK_RIGHT_CONTROL;
			case GLFW_KEY_RIGHT_ALT:		return KeyCode::SK_RIGHT_ALT;
			case GLFW_KEY_RIGHT_SUPER:		return KeyCode::SK_RIGHT_SUPER;
			case GLFW_KEY_MENU:				return KeyCode::SK_MENU;
			case GLFW_KEY_0:				return KeyCode::SK_0;
			case GLFW_KEY_1:				return KeyCode::SK_1;
			case GLFW_KEY_2:				return KeyCode::SK_2;
			case GLFW_KEY_3:				return KeyCode::SK_3;
			case GLFW_KEY_4:				return KeyCode::SK_4;
			case GLFW_KEY_5:				return KeyCode::SK_5;
			case GLFW_KEY_6:				return KeyCode::SK_6;
			case GLFW_KEY_7:				return KeyCode::SK_7;
			case GLFW_KEY_8:				return KeyCode::SK_8;
			case GLFW_KEY_9:				return KeyCode::SK_9;
			case GLFW_KEY_A:				return KeyCode::SK_A;
			case GLFW_KEY_B:				return KeyCode::SK_B;
			case GLFW_KEY_C:				return KeyCode::SK_C;
			case GLFW_KEY_D:				return KeyCode::SK_D;
			case GLFW_KEY_E:				return KeyCode::SK_E;
			case GLFW_KEY_F:				return KeyCode::SK_F;
			case GLFW_KEY_G:				return KeyCode::SK_G;
			case GLFW_KEY_H:				return KeyCode::SK_H;
			case GLFW_KEY_I:				return KeyCode::SK_I;
			case GLFW_KEY_J:				return KeyCode::SK_J;
			case GLFW_KEY_K:				return KeyCode::SK_K;
			case GLFW_KEY_L:				return KeyCode::SK_L;
			case GLFW_KEY_M:				return KeyCode::SK_M;
			case GLFW_KEY_N:				return KeyCode::SK_N;
			case GLFW_KEY_O:				return KeyCode::SK_O;
			case GLFW_KEY_P:				return KeyCode::SK_P;
			case GLFW_KEY_Q:				return KeyCode::SK_Q;
			case GLFW_KEY_R:				return KeyCode::SK_R;
			case GLFW_KEY_S:				return KeyCode::SK_S;
			case GLFW_KEY_T:				return KeyCode::SK_T;
			case GLFW_KEY_U:				return KeyCode::SK_U;
			case GLFW_KEY_V:				return KeyCode::SK_V;
			case GLFW_KEY_W:				return KeyCode::SK_W;
			case GLFW_KEY_X:				return KeyCode::SK_X;
			case GLFW_KEY_Y:				return KeyCode::SK_Y;
			case GLFW_KEY_Z:				return KeyCode::SK_Z;
			case GLFW_KEY_F1:				return KeyCode::SK_F1;
			case GLFW_KEY_F2:				return KeyCode::SK_F2;
			case GLFW_KEY_F3:				return KeyCode::SK_F3;
			case GLFW_KEY_F4:				return KeyCode::SK_F4;
			case GLFW_KEY_F5:				return KeyCode::SK_F5;
			case GLFW_KEY_F6:				return KeyCode::SK_F6;
			case GLFW_KEY_F7:				return KeyCode::SK_F7;
			case GLFW_KEY_F8:				return KeyCode::SK_F8;
			case GLFW_KEY_F9:				return KeyCode::SK_F9;
			case GLFW_KEY_F10:				return KeyCode::SK_F10;
			case GLFW_KEY_F11:				return KeyCode::SK_F11;
			case GLFW_KEY_F12:				return KeyCode::SK_F12;
			default:						return KeyCode::SK_NONE;
		}
	}

	MouseCode GlfwWindow::TranslateButton(int GlfwMouseButton)
	{
		switch (GlfwMouseButton)
		{
			case GLFW_MOUSE_BUTTON_LEFT:	return MouseCode::SM_LEFT;
			case GLFW_MOUSE_BUTTON_MIDDLE:	return MouseCode::SM_MIDDLE;
			case GLFW_MOUSE_BUTTON_RIGHT:	return MouseCode::SM_RIGHT;
			default:						return MouseCode::SM_NONE;
		}
	}

	void GlfwWindow::KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
	{
		GlfwWindow* pWindow = (GlfwWindow*)glfwGetWindowUserPointer(Window);

		if (Action == GLFW_PRESS)
		{
			KeyPressedEvent E(TranslateKey(Key));
			pWindow->m_EventCallbackFn(E);
		}
		else if (Action == GLFW_RELEASE)
		{
			KeyReleasedEvent E(TranslateKey(Key));
			pWindow->m_EventCallbackFn(E);
		}
	}

	void GlfwWindow::CharCallback(GLFWwindow* Window, uint32_t KeyCode)
	{
		GlfwWindow* pWindow = (GlfwWindow*)glfwGetWindowUserPointer(Window);
		KeyTypedEvent E((uint8_t)KeyCode);
		pWindow->m_EventCallbackFn(E);
	}

	void GlfwWindow::MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
	{
		GlfwWindow* pWindow = (GlfwWindow*)glfwGetWindowUserPointer(Window);

		if (Action == GLFW_PRESS)
		{
			MouseButtonPressedEvent E(TranslateButton(Button));
			pWindow->m_EventCallbackFn(E);
		}
		else if (Action == GLFW_RELEASE)
		{
			MouseButtonReleasedEvent E(TranslateButton(Button));
			pWindow->m_EventCallbackFn(E);
		}
	}

	void GlfwWindow::MouseScrollCallback(GLFWwindow* Window, double xOffset, double yOffset)
	{
		GlfwWindow* pWindow = (GlfwWindow*)glfwGetWindowUserPointer(Window);
		MouseScrolledEvent E((float)xOffset, (float)yOffset);
		pWindow->m_EventCallbackFn(E);
	}

	void GlfwWindow::MouseMovedCallback(GLFWwindow* Window, double xPos, double yPos)
	{
		GlfwWindow* pWindow = (GlfwWindow*)glfwGetWindowUserPointer(Window);
		MouseMovedEvent E((float)xPos, (float)yPos);
		pWindow->m_EventCallbackFn(E);
	}

	void GlfwWindow::WindowCloseCallback(GLFWwindow* Window)
	{
		GlfwWindow* pWindow = (GlfwWindow*)glfwGetWindowUserPointer(Window);
		pWindow->OnClose();
	}

}