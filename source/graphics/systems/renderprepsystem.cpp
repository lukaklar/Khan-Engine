#include "graphics/precomp.h"
#include "graphics/systems/renderprepsystem.hpp"
#include "graphics/components/lightcomponent.hpp"
#include "graphics/components/visualcomponent.hpp"
#include "graphics/graphicsmanager.hpp"
#include "graphics/objects/light.hpp"
#include "graphics/objects/mesh.hpp"
#include "graphics/renderer.hpp"
#include "core/camera/camera.hpp"
#include "core/camera/components/cameracomponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"

namespace Khan
{
	void RenderPrepSystem::Update(float dt) const
	{
		Entity* cameraEntity = nullptr;
		{
			auto group = World::GetCurrentWorld()->GetEntityGroup<CameraComponent>();

			for (Entity* entity : group->GetEntities())
			{
				// TODO: Find the active camera (the only one that should be updated and used in frustum culling and other calculations
				// which for now it is the first and only one)
				cameraEntity = entity;
				break;
			}
		}

		if (!cameraEntity) return;

		Camera* camera = cameraEntity->GetComponent<CameraComponent>().m_Camera;

		Renderer& renderer = GraphicsManager::Get()->GetRenderer();
		renderer.SetActiveCamera(camera);

		{
			auto group = World::GetCurrentWorld()->GetEntityGroup<VisualComponent>();

			for (Entity* entity : group->GetEntities())
			{
				if (entity->IsInFrustum())
				{
					VisualComponent& visual = entity->GetComponent<VisualComponent>();

					for (Mesh* mesh : visual.m_Meshes)
					{
						if (!camera->GetFrustum().Cull(mesh->m_AABB))
						{
							renderer.GetOpaqueMeshes().push_back(mesh);
						}
					}
				}
			}
		}

		{
			auto group = World::GetCurrentWorld()->GetEntityGroup<LightComponent>();

			for (Entity* entity : group->GetEntities())
			{
				const Light* light = entity->GetComponent<LightComponent>().m_Light;

				if (!light->IsActive())
				{
					continue;
				}

				ShaderLightData lightData;
				lightData.m_Color = light->GetColor();
				lightData.m_Luminance = light->GetLuminance();

				switch (light->GetType())
				{
					case Light::Type::Directional:
					{
						const DirectionalLight* directional = reinterpret_cast<const DirectionalLight*>(light);
						lightData.m_Type = Light::Type::Directional;
						const glm::quat& orientation = entity->GetGlobalOrientation();
						lightData.m_DirectionVS = (camera->GetViewMatrix() * glm::vec4(orientation.x, orientation.y, orientation.z, 0.0f)).xyz();
						break;
					}
					case Light::Type::Omni:
					{
						const OmniLight* omni = reinterpret_cast<const OmniLight*>(light);
						lightData.m_Type = Light::Type::Omni;
						lightData.m_PositionVS = (camera->GetViewMatrix() * entity->GetGlobalPosition()).xyz();
						lightData.m_Range = omni->GetRadius();
						break;
					}
					case Light::Type::Spot:
					{
						const SpotLight* spot = reinterpret_cast<const SpotLight*>(light);
						lightData.m_Type = Light::Type::Spot;
						const glm::quat& orientation = entity->GetGlobalOrientation();
						lightData.m_DirectionVS = (camera->GetViewMatrix() * glm::vec4(orientation.x, orientation.y, orientation.z, 0.0f)).xyz();
						lightData.m_PositionVS = (camera->GetViewMatrix() * entity->GetGlobalPosition()).xyz();
						lightData.m_SpotlightAngle = spot->GetAngle();
						lightData.m_Range = spot->GetRange();
						break;
					}
					case Light::Type::Area:
					{
						const AreaLight* area = reinterpret_cast<const AreaLight*>(light);
						lightData.m_Type = Light::Type::Area;
						break;
					}
				}

				renderer.GetActiveLightData().push_back(lightData);
			}
		}
	}
}