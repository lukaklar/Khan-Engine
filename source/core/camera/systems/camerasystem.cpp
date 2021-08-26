#include "core/precomp.h"
#include "core/camera/systems/camerasystem.hpp"
#include "core/camera/camera.hpp"
#include "core/camera/components/cameracomponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"

namespace Khan
{
	void CameraSystem::Update(float dt)
	{
		Entity* cameraEntity = nullptr;

		auto group = World::GetCurrentWorld()->GetEntityGroup<CameraComponent>();

		for (Entity* entity : group->GetEntities())
		{
			// TODO: Find the active camera (the only one that should be updated and used in frustum culling and other calculations
			// which for now it is the first and only one)
			cameraEntity = entity;
			break;
		}

		if (!cameraEntity) return;

		Camera* camera = cameraEntity->GetComponent<CameraComponent>().m_Camera;

		camera->Update(dt);

		for (Entity* entity : World::GetCurrentWorld()->GetEntities())
		{
			entity->SetInFrustum(!camera->GetFrustum().Cull(entity->GetBoundingVolume()));
		}
	}
}