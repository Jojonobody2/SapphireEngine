#include "AssimpModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Sapphire
{
	MeshData AssimpModelImporter::Import(const std::filesystem::path& Path)
	{
		MeshData Data{};

		Assimp::Importer Importer;
		const aiScene* pScene = Importer.ReadFile(Path.string(), aiProcess_Triangulate | aiProcess_FixInfacingNormals |
			aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		aiMesh* Mesh = pScene->mMeshes[0];
		for (uint32_t i = 0; i < Mesh->mNumVertices; i++)
		{
			Vertex Vertex{};

			aiVector3D Pos = Mesh->mVertices[i];
			Vertex.Pos = glm::vec3(Pos.x, Pos.y, Pos.z);

			aiVector3D Nml = Mesh->mNormals[i];
			Vertex.Nml = glm::vec3(Nml.x, Nml.y, Nml.z);

			Data.Vertices.push_back(Vertex);
		}

		for (uint32_t i = 0; i < Mesh->mNumFaces; i++)
		{
			aiFace Face = Mesh->mFaces[i];

			for (uint32_t j = 0; j < Face.mNumIndices; j++)
			{
				Data.Indices.push_back(Face.mIndices[j]);
			}
		}

		return Data;
	}	
}