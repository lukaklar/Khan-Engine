#include "data/precomp.h"
#include "data/datamanager.hpp"

#define KH_GFXAPI_VULKAN

#include "core/camera/camera.hpp"
#include "core/camera/components/cameracomponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"
#include "data/stb_image/stb_image.h"
#include "data/texturemanager.hpp"
#include "graphics/components/lightcomponent.hpp"
#include "graphics/components/visualcomponent.hpp"
#include "graphics/hal/buffer.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/renderbackend.hpp"
#include "graphics/hal/renderdevice.hpp"
#include "graphics/materials/material.hpp"
#include "graphics/objects/light.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/shadermanager.hpp"
#include "system/assert.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Khan
{
	DataManager::DataManager()
	{
		TextureManager::CreateSingleton();
		//m_Database.Open("");
	}

	DataManager::~DataManager()
	{
		for (auto& it : m_Meshes)
		{
			Mesh* mesh = it.second;

			RenderBackend::g_Device->DestroyBuffer(mesh->m_VertexBuffer);
			RenderBackend::g_Device->DestroyBuffer(mesh->m_IndexBuffer);
			delete mesh->m_Material;

			delete mesh;
		}

		//m_Database.Close();
		TextureManager::DestroySingleton();
	}

	World* DataManager::LoadWorldFromFile(const char* fileName)
	{
		std::string fullFileName = ms_AssetPath;
		fullFileName += fileName;

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fullFileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | aiProcess_TransformUVCoords);

		KH_ASSERT(scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode, importer.GetErrorString());

		World* world = new World(fileName);
		std::unordered_map<const aiNode*, Entity*> nodeToEntityMap;

		std::function<void(aiNode*,Entity*)> processNode = [this, scene, world, &nodeToEntityMap, &processNode](const aiNode* node, Entity* parent)
		{
			Entity* entity = world->CreateEntity();

			nodeToEntityMap[node] = entity;

			if (parent != nullptr)
			{
				parent->AddChild(entity);
			}

			entity->SetName(node->mName.C_Str());

			glm::mat4 transform = *reinterpret_cast<const glm::mat4*>(&node->mTransformation) * (parent ? parent->GetGlobalTransform() : glm::mat4(1.0f));
			entity->SetGlobalPosition(transform[3]);
			entity->SetGlobalOrientation(glm::toQuat(transform));
			entity->SetGlobalTransform(transform);

			if (node->mNumMeshes > 0)
			{
				VisualComponent& visualComponent = entity->AddComponent<VisualComponent>();

				BoundingVolume bv;
				bv.SetType(BoundingVolume::Type::AABBox);

				glm::vec3 bvMin(FLT_MAX);
				glm::vec3 bvMax(-FLT_MAX);

				for (uint32_t i = 0; i < node->mNumMeshes; ++i)
				{
					auto it = m_Meshes.find(node->mMeshes[i]);
					if (it != m_Meshes.end())
					{
						visualComponent.m_Meshes.push_back(it->second);
						continue;
					}

					const aiMesh& aimesh = *scene->mMeshes[node->mMeshes[i]];

					struct Vertex
					{
						glm::vec3 m_Position;
						glm::vec2 m_TexCoord;
						glm::vec3 m_Normal;
						glm::vec3 m_Tangent;
						glm::vec3 m_Bitangent;
					};

					std::vector<Vertex> vertices;
					std::vector<uint32_t> indices;

					BoundingVolume meshBV;
					meshBV.SetType(BoundingVolume::Type::AABBox);
					meshBV.SetParentMatrixPtr(&entity->GetGlobalTransform());

					glm::vec3 meshBVmin(FLT_MAX);
					glm::vec3 meshBVmax(-FLT_MAX);

					bool calcBV = aimesh.mAABB.mMin == aiVector3D(0) && aimesh.mAABB.mMax == aiVector3D(0);

					vertices.reserve(aimesh.mNumVertices);
					for (uint32_t i = 0; i < aimesh.mNumVertices; ++i)
					{
						Vertex vertex;
						vertex.m_Position = *reinterpret_cast<glm::vec3*>(&aimesh.mVertices[i]);
						vertex.m_TexCoord = *reinterpret_cast<glm::vec2*>(&aimesh.mTextureCoords[0][i]);
						vertex.m_Normal = *reinterpret_cast<glm::vec3*>(&aimesh.mNormals[i]);
						vertex.m_Tangent = *reinterpret_cast<glm::vec3*>(&aimesh.mTangents[i]);
						vertex.m_Bitangent = *reinterpret_cast<glm::vec3*>(&aimesh.mBitangents[i]);

						vertices.emplace_back(vertex);

						if (calcBV)
						{
							meshBVmin.x = std::min(meshBVmin.x, vertex.m_Position.x);
							meshBVmin.y = std::min(meshBVmin.y, vertex.m_Position.y);
							meshBVmin.z = std::min(meshBVmin.z, vertex.m_Position.z);

							meshBVmax.x = std::max(meshBVmax.x, vertex.m_Position.x);
							meshBVmax.y = std::max(meshBVmax.y, vertex.m_Position.y);
							meshBVmax.z = std::max(meshBVmax.z, vertex.m_Position.z);

							bvMin.x = std::min(bvMin.x, vertex.m_Position.x);
							bvMin.y = std::min(bvMin.y, vertex.m_Position.y);
							bvMin.z = std::min(bvMin.z, vertex.m_Position.z);

							bvMax.x = std::max(bvMax.x, vertex.m_Position.x);
							bvMax.y = std::max(bvMax.y, vertex.m_Position.y);
							bvMax.z = std::max(bvMax.z, vertex.m_Position.z);
						}
					}

					indices.reserve(aimesh.mNumFaces * 3);
					for (uint32_t i = 0; i < aimesh.mNumFaces; ++i)
					{
						const aiFace& face = aimesh.mFaces[i];
						
						indices.push_back(face.mIndices[0]);
						indices.push_back(face.mIndices[1]);
						indices.push_back(face.mIndices[2]);
					}

					if (!calcBV)
					{
						meshBVmin = *reinterpret_cast<const glm::vec3*>(&aimesh.mAABB.mMin);
						meshBVmax = *reinterpret_cast<const glm::vec3*>(&aimesh.mAABB.mMax);

						bvMin.x = std::min(bvMin.x, aimesh.mAABB.mMin.x);
						bvMin.y = std::min(bvMin.y, aimesh.mAABB.mMin.y);
						bvMin.z = std::min(bvMin.z, aimesh.mAABB.mMin.z);

						bvMax.x = std::max(bvMax.x, aimesh.mAABB.mMax.x);
						bvMax.y = std::max(bvMax.y, aimesh.mAABB.mMax.y);
						bvMax.z = std::max(bvMax.z, aimesh.mAABB.mMax.z);
					}

					meshBV.SetAABBoxMin(meshBVmin);
					meshBV.SetAABBoxMax(meshBVmax);

					Mesh* mesh = new Mesh();
					mesh->m_AABB = meshBV;

					{
						BufferDesc desc;
						desc.m_Size = aimesh.mNumVertices * sizeof(Vertex);
						desc.m_Flags = BufferFlag_AllowVertices | BufferFlag_Writable;

						mesh->m_VertexBuffer = RenderBackend::g_Device->CreateBuffer(desc, vertices.data());
						mesh->m_VertexCount = aimesh.mNumVertices;
					}

					{
						BufferDesc desc;
						desc.m_Size = aimesh.mNumFaces * 3 * sizeof(uint32_t);
						desc.m_Flags = BufferFlag_AllowIndices | BufferFlag_Writable;

						mesh->m_IndexBuffer = RenderBackend::g_Device->CreateBuffer(desc, indices.data());
						mesh->m_IndexCount = aimesh.mNumFaces * 3;
					}

					if (aimesh.mMaterialIndex >= 0)
					{
						const aiMaterial& mat = *scene->mMaterials[aimesh.mMaterialIndex];
						Material* material = new Material();
						aiString aiTexturePath;
						uint32_t binding = 0;
						bool hasNormalMap = false;

						if (mat.GetTextureCount(aiTextureType_DIFFUSE) > 0 && mat.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == aiReturn_SUCCESS)
						{
							std::string textureFilePath = ms_AssetPath;
							textureFilePath += aiTexturePath.C_Str();
							TextureView* texture = TextureManager::Get()->LoadTexture(textureFilePath.c_str());
							material->AddTexture(binding++, texture);
						}

						if (mat.GetTextureCount(aiTextureType_SPECULAR) > 0 && mat.GetTexture(aiTextureType_SPECULAR, 0, &aiTexturePath) == aiReturn_SUCCESS)
						{
							std::string textureFilePath = ms_AssetPath;
							textureFilePath += aiTexturePath.C_Str();
							TextureView* texture = TextureManager::Get()->LoadTexture(textureFilePath.c_str());
							material->AddTexture(binding++, texture);
						}

						if (mat.GetTextureCount(aiTextureType_NORMALS) > 0 && mat.GetTexture(aiTextureType_NORMALS, 0, &aiTexturePath) == aiReturn_SUCCESS)
						{
							std::string textureFilePath = ms_AssetPath;
							textureFilePath += aiTexturePath.C_Str();
							TextureView* texture = TextureManager::Get()->LoadTexture(textureFilePath.c_str());
							material->AddTexture(binding++, texture);
							hasNormalMap = true;
						}
						else if (mat.GetTextureCount(aiTextureType_HEIGHT) > 0 && mat.GetTexture(aiTextureType_HEIGHT, 0, &aiTexturePath) == aiReturn_SUCCESS)
						{
							std::string textureFilePath = ms_AssetPath;
							textureFilePath += aiTexturePath.C_Str();
							TextureView* texture = TextureManager::Get()->LoadTexture(textureFilePath.c_str());
							material->AddTexture(binding++, texture);
							hasNormalMap = true;
						}

						if (mat.GetTextureCount(aiTextureType_EMISSIVE) > 0 && mat.GetTexture(aiTextureType_EMISSIVE, 0, &aiTexturePath) == aiReturn_SUCCESS)
						{
							std::string textureFilePath = ms_AssetPath;
							textureFilePath += aiTexturePath.C_Str();
							TextureView* texture = TextureManager::Get()->LoadTexture(textureFilePath.c_str());
							material->AddTexture(binding++, texture);
						}

						material->SetTwoSided(false);
						material->SetTransparent(false);
						if (hasNormalMap)
						{
							material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("common_PS", "PS_Common"));
						}
						else
						{
							material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("common_no_normals_PS", "PS_CommonNoNormals"));
						}

						mesh->m_Material = material;
						mesh->m_ParentTransform.UpdateConstantData(&entity->GetGlobalTransform(), 0, sizeof(glm::mat4));

						m_Meshes.emplace(node->mMeshes[i], mesh);
						visualComponent.m_Meshes.push_back(mesh);
					}
				}

				bv.SetAABBoxMin(bvMin);
				bv.SetAABBoxMax(bvMax);
				bv.SetParentMatrixPtr(&entity->GetGlobalTransform());

				entity->SetBoundingVolume(bv);
			}

			for (uint32_t i = 0; i < node->mNumChildren; ++i)
			{
				processNode(node->mChildren[i], entity);
			}
		};

		processNode(scene->mRootNode, nullptr);

		Entity* entity = world->CreateEntity();
		LightComponent& lightComponent = entity->AddComponent<LightComponent>();
		DirectionalLight* directional = new DirectionalLight();
		directional->SetActive(true);
		directional->SetColor({ 1.0f, 0.0f, 1.0f });
		directional->SetLuminance(1.0f);
		directional->SetDirection({ -1, -1, 0 });
		lightComponent.m_Light = directional;

		for (uint32_t i = 0; i < scene->mNumLights; ++i)
		{
			const aiLight& light = *scene->mLights[i];
			const aiNode* lightNode = scene->mRootNode->FindNode(light.mName);

			Entity* entity = nodeToEntityMap[lightNode];
			LightComponent& lightComponent = entity->AddComponent<LightComponent>();

			switch (light.mType)
			{
				case aiLightSource_DIRECTIONAL:
				{
					DirectionalLight* directional = new DirectionalLight();
					directional->SetDirection(*reinterpret_cast<const glm::vec3*>(&light.mDirection));
					lightComponent.m_Light = directional;
					break;
				}
				case aiLightSource_POINT:
				{
					OmniLight* omni = new OmniLight();
					omni->SetRadius(100.0f);
					lightComponent.m_Light = omni;
					break;
				}
				case aiLightSource_SPOT:
				{
					SpotLight* spot = new SpotLight();
					// TODO: Take inner cone and adjust shader calculations for it
					spot->SetDirection(*reinterpret_cast<const glm::vec3*>(&light.mDirection));
					spot->SetAngle(light.mAngleOuterCone);
					spot->SetRange(100.0f);
					lightComponent.m_Light = spot;
					break;
				}
				case aiLightSource_AREA:
				{
					AreaLight* area = new AreaLight();
					area->SetAreaWidth(light.mSize.x);
					area->SetAreaHeight(light.mSize.y);
					lightComponent.m_Light = area;
				}
			}

			lightComponent.m_Light->SetActive(true);
			lightComponent.m_Light->SetColor(*reinterpret_cast<const glm::vec3*>(&light.mColorDiffuse));
			lightComponent.m_Light->SetLuminance(1.0f);
		}

		Entity* player = world->CreateEntity();
		player->SetParent(nodeToEntityMap[scene->mRootNode]);
		player->SetName("Player");
		player->SetGlobalPosition(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		player->SetGlobalOrientation(glm::quat(0.0f, 0.0f, 0.0f, 0.0f));
		player->SetGlobalTransform(glm::mat4(1.0f));

		CameraComponent& cameraComponent = player->AddComponent<CameraComponent>();
		Camera* camera = cameraComponent.m_Camera = new Camera();
		camera->SetTarget(player);

		// TODO: MovementComponent and input handling system and Camera updating (threadpool and dependencies between systems)

		RenderBackend::g_Device->WaitIdle();

		return world;
	}
}

#undef KH_GFXAPI_VULKAN