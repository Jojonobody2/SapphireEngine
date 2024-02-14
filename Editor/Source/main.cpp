#include <core/EntryPoint.h>
#include <core/Layer.h>

class BaseLayer : public Sapphire::Layer
{
	void OnAttach() override
	{

	}

	void OnUpdate() override
	{

	}
};

const Sapphire::ApplicationCreateInfo SapphireInitialize()
{
	Sapphire::ApplicationCreateInfo AppCI;
	AppCI.Name = "Sapphire Editor";
	AppCI.BaseLayer = new BaseLayer();

	return AppCI;
}