#include <core/EntryPoint.h>

#include <iostream>

const Sapphire::ApplicationCreateInfo SapphireInitialize()
{
	Sapphire::ApplicationCreateInfo AppCI{};
	AppCI.Name = "Sapphire Editor";

	return AppCI;
}