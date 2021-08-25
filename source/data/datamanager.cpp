#include "data/precomp.h"
#include "data/datamanager.hpp"

//#define KH_GFXAPI_VULKAN

#include "core/camera/camera.hpp"
#include "core/camera/components/cameracomponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"
#include "data/stb_image/stb_image.h"
#include "graphics/components/lightcomponent.hpp"
#include "graphics/components/visualcomponent.hpp"
#include "graphics/hal/pixelformats.hpp"
#include "graphics/hal/texture.hpp"
#include "graphics/hal/textureview.hpp"
#include "graphics/hal/renderbackend.hpp"
//#include "graphics/hal/vulkan/vulkandevice.hpp"
#include "graphics/objects/light.hpp"
#include "system/assert.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Khan
{
	DataManager::DataManager()
	{
		//m_Database.Open("");
	}

	DataManager::~DataManager()
	{
		//m_Database.Close();
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

		std::function<void(aiNode*,Entity*)> processNode = [scene, world, &nodeToEntityMap, &processNode](const aiNode* node, Entity* parent)
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
					const aiMesh& mesh = *scene->mMeshes[node->mMeshes[i]];
					std::vector<float> vertices;
					std::vector<uint32_t> indices;

					uint32_t vertexSizeInFloats = 3 + 2 + 3 + 3 + 3;
					vertices.reserve(vertexSizeInFloats * mesh.mNumVertices);
					for (uint32_t i = 0; i < mesh.mNumVertices; ++i)
					{

					}

					indices.reserve(mesh.mNumFaces * 3);
					for (uint32_t i = 0; i < mesh.mNumFaces; ++i)
					{
						const aiFace& face = mesh.mFaces[i];
						
						indices.push_back(face.mIndices[0]);
						indices.push_back(face.mIndices[1]);
						indices.push_back(face.mIndices[2]);
					}

					bvMin.x = std::min(bvMin.x, mesh.mAABB.mMin.x);
					bvMin.y = std::min(bvMin.y, mesh.mAABB.mMin.y);
					bvMin.z = std::min(bvMin.z, mesh.mAABB.mMin.z);

					bvMax.x = std::max(bvMax.x, mesh.mAABB.mMax.x);
					bvMax.y = std::max(bvMax.y, mesh.mAABB.mMax.y);
					bvMax.z = std::max(bvMax.z, mesh.mAABB.mMax.z);

					if (mesh.mMaterialIndex >= 0)
					{
						const aiMaterial& mat = *scene->mMaterials[mesh.mMaterialIndex];
						aiString aiTexturePath;
						uint32_t count = 0;

						// Load emissive textures.
						//if (mat.GetTextureCount(aiTextureType_EMISSIVE) > 0 &&
						//	mat.GetTexture(aiTextureType_EMISSIVE, 0, &aiTexturePath) == aiReturn_SUCCESS)
						//{
						//	fs::path texturePath(aiTexturePath.C_Str());
						//	std::shared_ptr<Texture> pTexture = CreateTexture((parentPath / texturePath).wstring());
						//	pMaterial->SetTexture(Material::TextureType::Emissive, pTexture);
						//}

						//// Load diffuse textures.
						//if (material.GetTextureCount(aiTextureType_DIFFUSE) > 0 &&
						//	material.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
						//{
						//	fs::path texturePath(aiTexturePath.C_Str());
						//	std::shared_ptr<Texture> pTexture = CreateTexture((parentPath / texturePath).wstring());
						//	pMaterial->SetTexture(Material::TextureType::Diffuse, pTexture);
						//}

						//// Load specular texture.
						//if (material.GetTextureCount(aiTextureType_SPECULAR) > 0 &&
						//	material.GetTexture(aiTextureType_SPECULAR, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation) == aiReturn_SUCCESS)
						//{
						//	fs::path texturePath(aiTexturePath.C_Str());
						//	std::shared_ptr<Texture> pTexture = CreateTexture((parentPath / texturePath).wstring());
						//	pMaterial->SetTexture(Material::TextureType::Specular, pTexture);
						//}

						//mat.GetTexture(aiTextureType_DIFFUSE, 0, &path);

						//mat.GetTexture()

						//mat.GetTexture(aiTextureType_HEIGHT, 0, &path);

						OutputDebugString("asdasda\n");
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

		return world;
	}

	//TextureView* DataManager::LoadTextureFromFile(const char* fileName)
	//{
	//	int32_t width, height, channels;
	//	unsigned char* data = stbi_load(fileName, &width, &height, &channels, STBI_rgb_alpha);

	//	Texture* texture;
	//	{
	//		TextureDesc desc;
	//		desc.m_Type = TextureType_2D;
	//		desc.m_Width = static_cast<uint32_t>(width);
	//		desc.m_Height = static_cast<uint32_t>(height);
	//		desc.m_Depth = 1;
	//		desc.m_ArrayLayers = 1;
	//		desc.m_MipLevels = 1;
	//		desc.m_Format = PF_R8G8B8A8_UNORM;
	//		desc.m_Flags = TextureFlag_AllowUnorderedAccess;

	//		texture = RenderBackend::g_Device->CreateTexture(desc);
	//	}

	//	// TODO: Upload texture data

	//	TextureView* view;
	//	{
	//		TextureViewDesc desc;
	//		desc.m_Type = TextureViewType_2D;
	//		desc.m_Format = PF_R8G8B8A8_UNORM;
	//		desc.m_BaseArrayLayer = 0;
	//		desc.m_LayerCount = 1;
	//		desc.m_BaseMipLevel = 0;
	//		desc.m_LevelCount = 1;

	//		view = RenderBackend::g_Device->CreateTextureView(texture, desc);
	//	}

	//	return view;
	//}
}

//#undef KH_GFXAPI_VULKAN