#include "data/precomp.h"
#include "data/datamanager.hpp"

#define KH_GFXAPI_VULKAN

#include "core/camera/camera.hpp"
#include "core/camera/components/cameracomponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"
#include "data/stb_image/stb_image.h"
#include "data/texturemanager.hpp"
#include "engine/components/motioncomponent.hpp"
#include "graphics/components/lightcomponent.hpp"
#include "graphics/components/visualcomponent.hpp"
#include "graphics/materials/material.hpp"
#include "graphics/objects/light.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/shadermanager.hpp"
#include "graphicshal/buffer.hpp"
#include "graphicshal/pixelformats.hpp"
#include "graphicshal/renderbackend.hpp"
#include "graphicshal/renderdevice.hpp"
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
		RenderBackend::g_Device->WaitIdle();

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
		const aiScene* scene = importer.ReadFile(fullFileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_TransformUVCoords);

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

			//glm::mat4 transform = *reinterpret_cast<const glm::mat4*>(&node->mTransformation) * (parent ? parent->GetGlobalTransform() : glm::mat4(1.0f));
			float scale = 0.5f;
			glm::mat4 transform = glm::scale(glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::vec3(scale, scale, scale));
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
							meshBVmin.x = std::min(meshBVmin.x, vertex.m_Position.x * scale);
							meshBVmin.y = std::min(meshBVmin.y, vertex.m_Position.y * scale);
							meshBVmin.z = std::min(meshBVmin.z, vertex.m_Position.z * scale);

							meshBVmax.x = std::max(meshBVmax.x, vertex.m_Position.x * scale);
							meshBVmax.y = std::max(meshBVmax.y, vertex.m_Position.y * scale);
							meshBVmax.z = std::max(meshBVmax.z, vertex.m_Position.z * scale);

							bvMin.x = std::min(bvMin.x, vertex.m_Position.x * scale);
							bvMin.y = std::min(bvMin.y, vertex.m_Position.y * scale);
							bvMin.z = std::min(bvMin.z, vertex.m_Position.z * scale);

							bvMax.x = std::max(bvMax.x, vertex.m_Position.x * scale);
							bvMax.y = std::max(bvMax.y, vertex.m_Position.y * scale);
							bvMax.z = std::max(bvMax.z, vertex.m_Position.z * scale);
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

						bvMin.x = std::min(bvMin.x, aimesh.mAABB.mMin.x * scale);
						bvMin.y = std::min(bvMin.y, aimesh.mAABB.mMin.y * scale);
						bvMin.z = std::min(bvMin.z, aimesh.mAABB.mMin.z * scale);

						bvMax.x = std::max(bvMax.x, aimesh.mAABB.mMax.x * scale);
						bvMax.y = std::max(bvMax.y, aimesh.mAABB.mMax.y * scale);
						bvMax.z = std::max(bvMax.z, aimesh.mAABB.mMax.z * scale);
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

						if (mat.GetTextureCount(aiTextureType_DIFFUSE) > 0 && mat.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == aiReturn_SUCCESS)
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
						}
						else if (mat.GetTextureCount(aiTextureType_HEIGHT) > 0 && mat.GetTexture(aiTextureType_HEIGHT, 0, &aiTexturePath) == aiReturn_SUCCESS)
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

						std::string textureFilePath = ms_AssetPath;
						textureFilePath += "roughness.jpg";
						TextureView* texture = TextureManager::Get()->LoadTexture(textureFilePath.c_str());
						material->AddTexture(binding++, texture);

						material->SetTwoSided(false);
						material->SetTransparent(false);

						switch (binding)
						{
						case 1:
							material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("common_diff_only_PS", "PS_CommonDiffuseOnly"));
							break;
						case 2:
							material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("common_no_normals_PS", "PS_CommonNoNormals"));
							break;
						case 4:
							material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("common_PS", "PS_Common"));
							break;
						default:
							material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("gbuffer_test_PS", "PS_GBufferTest"));
							break;
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
					lightComponent.m_Light = new DirectionalLight();
					entity->SetGlobalOrientation(glm::quat(0.0f, light.mDirection.x, light.mDirection.y, light.mDirection.z));
					break;
				}
				case aiLightSource_POINT:
				{
					OmniLight* omni = new OmniLight();
					entity->SetGlobalPosition(glm::vec4(light.mPosition.x, light.mPosition.y, light.mPosition.z, 1.0f));
					omni->SetRadius(1.0f);
					omni->SetAttenuation({ light.mAttenuationConstant, light.mAttenuationLinear, light.mAttenuationQuadratic });
					lightComponent.m_Light = omni;
					break;
				}
				case aiLightSource_SPOT:
				{
					SpotLight* spot = new SpotLight();
					entity->SetGlobalPosition(glm::vec4(light.mPosition.x, light.mPosition.y, light.mPosition.z, 1.0f));
					entity->SetGlobalOrientation(glm::quat(0.0f, light.mDirection.x, light.mDirection.y, light.mDirection.z));
					spot->SetRange(1.0f);
					spot->SetInnerConeAngle(light.mAngleInnerCone);
					spot->SetOuterConeAngle(light.mAngleOuterCone);
					spot->SetAttenuation({ light.mAttenuationConstant, light.mAttenuationLinear, light.mAttenuationQuadratic });
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
		player->SetParent(nullptr);
		player->SetName("Player");
		player->SetGlobalPosition(glm::vec4(0.0f, 0.0f, 3.0f, 1.0f));
		player->SetGlobalOrientation(glm::quat(0.0f, 0.0f, 0.0f, -1.0f));
		player->SetGlobalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 3.0f))* glm::toMat4(player->GetGlobalOrientation()));

		CameraComponent& cameraComponent = player->AddComponent<CameraComponent>();
		Camera* camera = cameraComponent.m_Camera = new Camera();
		camera->SetTarget(player);

		MotionComponent& motionComponent = player->AddComponent<MotionComponent>();
		motionComponent.m_MovementSpeed = 1.0f;
		motionComponent.m_RotationSpeed = 0.2f;

		/*{
			LightComponent& lightComponent = player->AddComponent<LightComponent>();
			OmniLight* light = new OmniLight();
			light->SetActive(true);
			light->SetColor({ 0.0f, 1.0f, 0.0f });
			light->SetLuminance(5.0f);
			light->SetRadius(2.0f);
			light->SetAttenuation({ 0.0f, 0.0f, 5.0f });
			lightComponent.m_Light = light;
		}*/

		{
			LightComponent& lightComponent = player->AddComponent<LightComponent>();
			SpotLight* light = new SpotLight();
			light->SetActive(true);
			light->SetColor({ 0.0f, 1.0f, 0.0f });
			light->SetLuminance(5.0f);
			light->SetRange(2.0f);
			light->SetAttenuation({ 0.0f, 0.0f, 1.0f });
			light->SetInnerConeAngle(glm::radians(12.5f));
			light->SetOuterConeAngle(glm::radians(15.0f));
			lightComponent.m_Light = light;
		}

		{
			Entity* lightEntity = world->CreateEntity();
			lightEntity->SetGlobalOrientation(glm::quat(0.0f, 1.0f, 0.0f, 0.0f));
			LightComponent& lightComponent = lightEntity->AddComponent<LightComponent>();
			DirectionalLight* light = new DirectionalLight();
			light->SetActive(true);
			light->SetColor({ 0.9922f, 0.9843f, 0.8275f });
			light->SetLuminance(10.0f);
			lightComponent.m_Light = light;
		}

		RenderBackend::g_Device->WaitIdle();

		return world;
	}

	World* DataManager::CreateTestPlayground()
	{
		World* world = new World("Test playground");

		std::vector<float> vertices =
		{
			// vertex 0
			-1.0f, 1.0f, -1.0f,		// position
			0.0f, 0.0f,				// texCoord
			0.0f, 0.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent

			// vertex 1
			1.0f, 1.0f, -1.0f,
			0.0f, 0.0f,				// texCoord
			0.0f, 1.0f, 0.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent

			// vertex 2
			-1.0f, -1.0f, -1.0f,
			0.0f, 0.0f,				// texCoord
			1.0f, 0.0f, 0.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent

			// vertex 3
			1.0f, -1.0f, -1.0f,
			0.0f, 0.0f,				// texCoord
			0.0f, 1.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent

			// vertex 4
			-1.0f, 1.0f, 1.0f,
			0.0f, 0.0f,				// texCoord
			0.0f, 0.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent

			// vertex 5
			1.0f, 1.0f, 1.0f,
			0.0f, 0.0f,				// texCoord
			1.0f, 0.0f, 0.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent

			// vertex 6
			-1.0f, -1.0f, 1.0f,
			0.0f, 0.0f,				// texCoord
			0.0f, 1.0f, 0.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent

			// vertex 7
			1.0f, -1.0f, 1.0f,
			0.0f, 0.0f,				// texCoord
			0.0f, 1.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 0.0f, 0.0f,		// tangent
			0.0f, 0.0f, 0.0f,		// bitangent
		};

		std::vector<uint32_t> indices =
		{
			0, 1, 2,    // side 1
			2, 1, 3,
			4, 0, 6,    // side 2
			6, 0, 2,
			7, 5, 6,    // side 3
			6, 5, 4,
			3, 1, 7,    // side 4
			7, 1, 5,
			4, 5, 0,    // side 5
			0, 5, 1,
			3, 7, 2,    // side 6
			2, 7, 6,
		};

		Mesh* mesh = new Mesh();

		{
			BufferDesc desc;
			desc.m_Size = vertices.size() * sizeof(float);
			desc.m_Flags = BufferFlag_AllowVertices | BufferFlag_Writable;

			mesh->m_VertexBuffer = RenderBackend::g_Device->CreateBuffer(desc, vertices.data());
			mesh->m_VertexCount = vertices.size() / 14;
		}

		{
			BufferDesc desc;
			desc.m_Size = indices.size() * sizeof(uint32_t);
			desc.m_Flags = BufferFlag_AllowIndices | BufferFlag_Writable;

			mesh->m_IndexBuffer = RenderBackend::g_Device->CreateBuffer(desc, indices.data());
			mesh->m_IndexCount = indices.size();
		}

		float scale = 0.05f;

		Entity* model = world->CreateEntity();
		model->SetGlobalPosition({ 0, 0, 0.5f, 1 });
		model->SetGlobalOrientation({ 0, 0, 0, 0 });
		glm::mat4 transform = glm::scale(glm::rotate(glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(scale, scale, scale));
		model->SetGlobalTransform(transform);

		BoundingVolume bv;
		bv.SetType(BoundingVolume::Type::OOBBox);
		bv.SetOOBBoxMin(glm::vec3(-1.0f, -1.0f, -1.0f));
		bv.SetOOBBoxMax(glm::vec3(1.0f, 1.0f, 1.0f));
		bv.SetParentMatrixPtr(&model->GetGlobalTransform());

		model->SetBoundingVolume(bv);

		mesh->m_AABB = bv;

		Material* material = new Material();
		material->SetTwoSided(false);
		material->SetTransparent(false);
		//material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("test_PS", "PS_Test"));
		material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("gbuffer_test_PS", "PS_GBufferTest"));

		mesh->m_Material = material;
		mesh->m_ParentTransform.UpdateConstantData(&model->GetGlobalTransform(), 0, sizeof(glm::mat4));

		VisualComponent& visual = model->AddComponent<VisualComponent>();
		visual.m_Meshes.push_back(mesh);
		m_Meshes.insert({ 0, mesh });

		{
			Entity* entity = world->CreateEntity();
			entity->SetGlobalOrientation(glm::quat(0.0f, -1.0f, -1.0f, 0.0f));

			LightComponent& lightComponent = entity->AddComponent<LightComponent>();
			DirectionalLight* directional = new DirectionalLight();
			directional->SetActive(true);
			directional->SetColor({ 0.0f, 0.0f, 0.5f });
			directional->SetLuminance(1.0f);
			lightComponent.m_Light = directional;
		}

		{
			Entity* entity = world->CreateEntity();
			entity->SetGlobalPosition(glm::vec4(0, 0, 0, 1));

			LightComponent& lightComponent = entity->AddComponent<LightComponent>();
			OmniLight* omni = new OmniLight();
			omni->SetActive(true);
			omni->SetColor({ 1.0f, 1.0f, 0.0f });
			omni->SetLuminance(1.0f);
			omni->SetRadius(1.0f);
			lightComponent.m_Light = omni;
		}

		/*{
			Entity* entity = world->CreateEntity();
			entity->SetGlobalPosition(glm::vec4(0, 0, 0, 1));

			LightComponent& lightComponent = entity->AddComponent<LightComponent>();
			SpotLight* spot = new SpotLight();
			spot->SetActive(true);
			spot->SetColor({ 1.0f, 1.0f, 0.0f });
			spot->SetLuminance(1.0f);
			spot->SetDirection(glm::vec3(0.0f, 0.0f, 1.0f));
			spot->SetAngle(0.4f);
			spot->SetRange(1.0f);
			lightComponent.m_Light = spot;
		}*/

		/*{
			Entity* entity = world->CreateEntity();

			LightComponent& lightComponent = entity->AddComponent<LightComponent>();
			DirectionalLight* directional = new DirectionalLight();
			directional->SetActive(true);
			directional->SetColor({ 0.7f, 0.6f, 0.2f });
			directional->SetLuminance(1.0f);
			directional->SetDirection({ 1, 1, 0 });
			lightComponent.m_Light = directional;
		}*/

		Entity* player = world->CreateEntity();
		player->SetName("Player");
		player->SetGlobalPosition(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		player->SetGlobalOrientation(glm::quat(0.0f, 0.0f, 0.0f, -1.0f));
		player->SetGlobalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f))* glm::toMat4(player->GetGlobalOrientation()));

		CameraComponent& cameraComponent = player->AddComponent<CameraComponent>();
		Camera* camera = cameraComponent.m_Camera = new Camera();
		camera->SetTarget(player);

		MotionComponent& motionComponent = player->AddComponent<MotionComponent>();
		motionComponent.m_MovementSpeed = 1.0f;
		motionComponent.m_RotationSpeed = 0.2f;

		return world;
	}

	World* DataManager::CreateTestPlayground2()
	{
		World* world = new World("Test playground");

		std::vector<float> vertices =
		{
			// vertex 0
			-1.0f, 1.0f, 0.0f,		// position
			0.0f, 0.0f,				// texCoord
			0.0f, 0.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 1.0f, 0.0f,		// tangent
			1.0f, 0.0f, 0.0f,		// bitangent

			// vertex 1
			1.0f, 1.0f, 0.0f,
			1.0f, 0.0f,				// texCoord
			0.0f, 0.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 1.0f, 0.0f,		// tangent
			1.0f, 0.0f, 0.0f,		// bitangent

			// vertex 2
			-1.0f, -1.0f, 0.0f,
			0.0f, 1.0f,				// texCoord
			0.0f, 0.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 1.0f, 0.0f,		// tangent
			1.0f, 0.0f, 0.0f,		// bitangent

			// vertex 3
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f,				// texCoord
			0.0f, 0.0f, 1.0f,		// normal (acting as color for now)
			0.0f, 1.0f, 0.0f,		// tangent
			1.0f, 0.0f, 0.0f,		// bitangent
		};

		std::vector<uint32_t> indices =
		{
			0, 2, 1,
			1, 2, 3
		};

		Mesh* mesh = new Mesh();

		{
			BufferDesc desc;
			desc.m_Size = vertices.size() * sizeof(float);
			desc.m_Flags = BufferFlag_AllowVertices | BufferFlag_Writable;

			mesh->m_VertexBuffer = RenderBackend::g_Device->CreateBuffer(desc, vertices.data());
			mesh->m_VertexCount = vertices.size() / 14;
		}

		{
			BufferDesc desc;
			desc.m_Size = indices.size() * sizeof(uint32_t);
			desc.m_Flags = BufferFlag_AllowIndices | BufferFlag_Writable;

			mesh->m_IndexBuffer = RenderBackend::g_Device->CreateBuffer(desc, indices.data());
			mesh->m_IndexCount = indices.size();
		}

		float scale = 0.25f;

		Entity* model = world->CreateEntity();
		model->SetGlobalPosition({ 0, 0, 0.5f, 1 });
		model->SetGlobalOrientation({ 0, 0, 0, 0 });
		//glm::mat4 transform = glm::scale(glm::rotate(glm::rotate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f)), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(scale, scale, scale));
		glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f)), glm::vec3(scale, scale, scale));
		model->SetGlobalTransform(transform);

		BoundingVolume bv;
		bv.SetType(BoundingVolume::Type::OOBBox);
		bv.SetOOBBoxMin(glm::vec3(-1.0f, -1.0f, -1.0f));
		bv.SetOOBBoxMax(glm::vec3(1.0f, 1.0f, 1.0f));
		bv.SetParentMatrixPtr(&model->GetGlobalTransform());

		model->SetBoundingVolume(bv);

		mesh->m_AABB = bv;

		Material* material = new Material();
		material->SetTwoSided(false);
		material->SetTransparent(false);
		//material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("test_PS", "PS_Test"));
		material->SetPixelShader(ShaderManager::Get()->GetShader<ShaderType_Pixel>("common_diff_only_PS", "PS_CommonDiffuseOnly"));

		mesh->m_Material = material;
		mesh->m_ParentTransform.UpdateConstantData(&model->GetGlobalTransform(), 0, sizeof(glm::mat4));
		material->AddTexture(0, TextureManager::Get()->LoadTexture("..\\..\\assets\\test.jpg"));

		VisualComponent& visual = model->AddComponent<VisualComponent>();
		visual.m_Meshes.push_back(mesh);
		m_Meshes.insert({ 0, mesh });

		{
			Entity* entity = world->CreateEntity();
			entity->SetGlobalOrientation(glm::quat(0.0f, -1.0f, -1.0f, 0.0f));

			LightComponent& lightComponent = entity->AddComponent<LightComponent>();
			DirectionalLight* directional = new DirectionalLight();
			directional->SetActive(true);
			directional->SetColor({ 1.0f, 1.0f, 1.0f });
			directional->SetLuminance(1.0f);
			lightComponent.m_Light = directional;
		}

		Entity* player = world->CreateEntity();
		player->SetName("Player");
		player->SetGlobalPosition(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		player->SetGlobalOrientation(glm::quat(0.0f, 0.0f, 0.0f, -1.0f));
		player->SetGlobalTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::toMat4(player->GetGlobalOrientation()));

		CameraComponent& cameraComponent = player->AddComponent<CameraComponent>();
		Camera* camera = cameraComponent.m_Camera = new Camera();
		camera->SetTarget(player);

		MotionComponent& motionComponent = player->AddComponent<MotionComponent>();
		motionComponent.m_MovementSpeed = 1.0f;
		motionComponent.m_RotationSpeed = 0.2f;

		return world;
	}
}

#undef KH_GFXAPI_VULKAN