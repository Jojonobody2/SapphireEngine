#include "RenderEngine.h"

#include "Renderer.h"

namespace Sapphire
{
	Renderer VulkanRenderer{};

	void RenderEngine::Init()
	{
		VulkanRenderer.Init();
	}

	void RenderEngine::Submit()
	{
		VulkanRenderer.Draw();
	}

	void RenderEngine::Shutdown()
	{
		VulkanRenderer.Shutdown();
	}
}