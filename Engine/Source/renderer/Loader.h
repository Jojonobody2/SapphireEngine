#pragma once

#include "Types.h"
#include "Renderer.h"

#include <unordered_map>
#include <filesystem>

namespace Sapphire
{
	std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGLTFMesh(Renderer* RenderEngine, std::filesystem::path Path);
}