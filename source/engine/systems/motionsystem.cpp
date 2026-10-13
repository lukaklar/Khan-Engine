#include "engine/precomp.h"
#include "engine/systems/motionsystem.hpp"
#include "engine/components/motioncomponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"
#include "core/math.h"
#include "system/input/inputmanager.hpp"

namespace Khan
{
	void MotionSystem::Update(float dt) const
	{
		auto group = World::GetCurrentWorld()->GetEntityGroup<MotionComponent>();

		for (Entity* entity : group->GetEntities())
		{
			const MotionComponent& motion = entity->GetComponent<MotionComponent>();
			const Quaternion& orientation = entity->GetGlobalOrientation();
			Vector3 direction = Normalize(Vector3(orientation.x, orientation.y, orientation.z));
			Vector3 position = entity->GetGlobalPosition().xyz();
			InputManager* inputMgr = InputManager::Get();
			Vector2I cursorDelta = inputMgr->GetCursorDelta();
			Vector3 up(0.0f, 1.0f, 0.0f);

			Vector3 strafeDirection = Normalize(Cross(direction, up));
			direction = Normalize(Matrix3(Rotate(Radians(-cursorDelta.x * motion.m_RotationSpeed), up) * Rotate(Radians(-cursorDelta.y * motion.m_RotationSpeed), strafeDirection)) * direction);			

			if (inputMgr->IsActionActive(InputAction::MoveForward))
			{
				position += motion.m_MovementSpeed * direction * dt;
			}
			if (inputMgr->IsActionActive(InputAction::MoveBack))
			{
				position -= motion.m_MovementSpeed * direction * dt;
			}
			if (inputMgr->IsActionActive(InputAction::MoveRight))
			{
				position += motion.m_MovementSpeed * strafeDirection * dt;
			}
			if (inputMgr->IsActionActive(InputAction::MoveLeft))
			{
				position -= motion.m_MovementSpeed * strafeDirection * dt;
			}
			if (inputMgr->IsActionActive(InputAction::MoveUp))
			{
				position += motion.m_MovementSpeed * up * dt;
			}
			if (inputMgr->IsActionActive(InputAction::MoveDown))
			{
				position -= motion.m_MovementSpeed * up * dt;
			}

			entity->SetGlobalPosition(Vector4(position.x, position.y, position.z, 1.0f));
			entity->SetGlobalOrientation(Quaternion(0.0f, direction));
			entity->SetGlobalTransform(Translate(Matrix4(1.0f), position) * ToMatrix4(entity->GetGlobalOrientation()));

			inputMgr->ResetCursorDelta();
		}
	}
}