#include <core/EntryPoint.h>
#include <core/Layer.h>

class BaseLayer : public Sapphire::Layer
{
	void OnAttach() override
	{
		Sapphire::Application::Get().GetWindow()->SetIcon(
			Sapphire::Image::Load("Resources/Icons/icon.png"), Sapphire::Image::Load("Resources/Icons/icon_sm.png"));
	}

	void OnUpdate() override
	{

	}

	void OnDetach() override
	{

	}
};

const Sapphire::ApplicationCreateInfo SapphireInitialize()
{
	Sapphire::ApplicationCreateInfo AppCI{};
	AppCI.Name = "Sapphire Editor";
	AppCI.BaseLayer = std::make_shared<BaseLayer>();

	return AppCI;
}