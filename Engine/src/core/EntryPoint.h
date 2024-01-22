#pragma once

#include "Application.h"

extern const Sapphire::ApplicationCreateInfo SapphireInitialize();

int main()
{
	Sapphire::Application* App = new Sapphire::Application(SapphireInitialize());
	App->Run();
	delete App;
}