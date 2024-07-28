#include "AssimpModelImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/GltfMaterial.h>

#include "Sapphire/Logging/Logger.h"

namespace Sapphire
{
	ModelData AssimpModelImporter::Import(const std::filesystem::path& Path)
	{
		std::vector<MeshData> MeshDatas{};

		Assimp::Importer Importer;
		const aiScene* pScene = Importer.ReadFile(Path.string(), aiProcess_Triangulate | aiProcess_FixInfacingNormals |
			aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices);

		for (uint32_t k = 0; k < pScene->mNumMeshes; k++)
		{
			MeshData Data{};

			aiMesh* Mesh = pScene->mMeshes[k];

			for (uint32_t i = 0; i < Mesh->mNumVertices; i++)
			{
				Vertex Vertex{};

				aiVector3D Pos = Mesh->mVertices[i];
				aiVector3D Nml = Mesh->mNormals[i];
				aiVector3D UV = Mesh->mTextureCoords[0][i];
                aiVector3D Tan = Mesh->mTangents[i];
                aiVector3D Bitan = Mesh->mBitangents[i];

				Vertex.PosUVx = glm::vec4(Pos.x, Pos.y, Pos.z, UV.x);
				Vertex.NmlUVy = glm::vec4(Nml.x, Nml.y, Nml.z, UV.y);
                Vertex.Tan = glm::vec3(Tan.x, Tan.y, Tan.z);
                Vertex.Bitan = glm::vec3(Bitan.x, Bitan.y, Bitan.z);

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

			Data.MaterialIndex = Mesh->mMaterialIndex;

			MeshDatas.push_back(Data);
		}

		std::vector<MaterialData> MaterialDatas{};

		for (uint32_t i = 0; i < pScene->mNumMaterials; i++)
		{
			aiMaterial* Material = pScene->mMaterials[i];

			aiString DiffuseTexPath;
			aiGetMaterialTexture(Material, aiTextureType_DIFFUSE, 0, &DiffuseTexPath);

            aiString NormalTexPath;
            aiGetMaterialTexture(Material, aiTextureType_NORMALS, 0, &NormalTexPath);

            aiString PBRTexPath;
            aiGetMaterialTexture(Material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &PBRTexPath);

			MaterialDatas.push_back({ DiffuseTexPath.C_Str(), NormalTexPath.C_Str(), PBRTexPath.C_Str(), i });
		}

		return { MeshDatas, MaterialDatas };
	}	
}