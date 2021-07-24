#include "core/precomp.h"
#include "core/camera/systems/frustumcullingsystem.hpp"
#include "core/camera/components/cameracomponent.hpp"
#include "core/ecs/entity.hpp"

namespace Khan
{
	void FrustumCullingSystem::Update(float dt)
	{
		Camera* camera = nullptr;

		{
			auto group = m_World.GetEntityGroup<CameraComponent>();

			for (Entity* entity : group->GetEntites())
			{
				camera = entity->GetComponent<CameraComponent>().m_Camera;
				break;
			}
		}

		if (!camera) return;

		// TODO: Update the camera

		for (Entity* entity : m_World.GetEntities())
		{
			entity->SetInFrustum(!camera->GetFrustum().Cull(entity->GetBoundingVolume()));
		}
	}
}