#pragma once

#include "Sapphire/Import/Model/ModelImporter.h"

namespace Sapphire
{
	class AssimpModelImporter
	{
	public:
		static MeshData Import(const std::filesystem::path& Path);
	};
}