#pragma once

#include "Application.h"

extern Sapphire::ApplicationInfo SapphireInitialize();

int main()
{
    auto AppInfo = SapphireInitialize();

    auto* App = new Sapphire::Application(AppInfo);
    App->Run();
    delete App;
}