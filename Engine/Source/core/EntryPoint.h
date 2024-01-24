#pragma once

#include "Application.h"

#ifdef SAPPHIRE_WINDOWS
#include <Windows.h>
#endif

extern const Sapphire::ApplicationCreateInfo SapphireInitialize();

#ifdef SAPPHIRE_WINDOWS
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#else
int main()
#endif
{
	Sapphire::Application* App = new Sapphire::Application(SapphireInitialize());
	App->Run();
	delete App;
}