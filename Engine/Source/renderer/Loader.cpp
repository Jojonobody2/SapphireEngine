#include "Loader.h"

#include "Renderer.h"
#include "VkUtil.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/glm_element_traits.hpp>

namespace Sapphire
{
	std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadGLTFMesh(Renderer* RenderEngine, std::filesystem::path Path)
	{
		fastgltf::GltfDataBuffer Data;
		Data.loadFromFile(Path);

		constexpr auto LoaderOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;

		fastgltf::Asset GLTF;
		fastgltf::Parser Parser;

		auto Load = Parser.loadGltf(&Data, Path.parent_path(), LoaderOptions);

		if (Load)
		{
			GLTF = std::move(Load.get());
		}
		else
		{
			SAPPHIRE_ERROR("Failed to load GLTF Asset: {}\n", fastgltf::to_underlying(Load.error()));
			return {};
		}

		std::vector<std::shared_ptr<MeshAsset>> Meshes;

		std::vector<uint32_t> Indices;
		std::vector<Vertex> Vertices;

		for (fastgltf::Mesh& Mesh : GLTF.meshes)
		{
			MeshAsset NewMesh;

			NewMesh.Name = Mesh.name;

			Indices.clear();
			Vertices.clear();

			for (auto&& P : Mesh.primitives)
			{
				GeoSurface NewSurface{};

				NewSurface.StartIndex = (uint32_t)Indices.size();
				NewSurface.Count = (uint32_t)GLTF.accessors[P.indicesAccessor.value()].count;

				size_t InitialVertex = Vertices.size();

				{
					fastgltf::Accessor& IndexAccessor = GLTF.accessors[P.indicesAccessor.value()];
					Indices.reserve(Indices.size() + IndexAccessor.count);

					fastgltf::iterateAccessor<std::uint32_t>(GLTF, IndexAccessor, [&](std::uint32_t Index)
						{
							Indices.push_back(Index + (uint32_t)InitialVertex);
						});
				}

				{
					fastgltf::Accessor& PosAccessor = GLTF.accessors[P.findAttribute("POSITION")->second];
					Vertices.resize(Vertices.size() + PosAccessor.count);

					fastgltf::iterateAccessorWithIndex<glm::vec3>(GLTF, PosAccessor, [&](glm::vec3 Pos, size_t Index)
						{
							Vertex NewVertex{};
							NewVertex.Position = Pos;
							Vertices[InitialVertex + Index] = NewVertex;
						});
				}

				NewMesh.Surfaces.push_back(NewSurface);
			}

			NewMesh.Meshbuffer = RenderEngine->UploadMesh(Vertices, Indices);
			Meshes.emplace_back(std::make_shared<MeshAsset>(std::move(NewMesh)));
		}

		return Meshes;
	}
}