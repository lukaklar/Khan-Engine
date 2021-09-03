#include "engine/precomp.h"
#include "engine/systems/motionsystem.hpp"
#include "engine/components/motioncomponent.hpp"
#include "core/ecs/entity.hpp"
#include "core/ecs/world.hpp"
#include "system/input/inputmanager.hpp"
#include <thirdparty/glm/gtx/transform.hpp>

namespace Khan
{
	void MotionSystem::Update(float dt) const
	{
		auto group = World::GetCurrentWorld()->GetEntityGroup<MotionComponent>();

		for (Entity* entity : group->GetEntities())
		{
			const MotionComponent& motion = entity->GetComponent<MotionComponent>();
			const glm::quat& orientation = entity->GetGlobalOrientation();
			glm::vec3 direction = glm::normalize(glm::vec3(orientation.x, orientation.y, orientation.z));
			glm::vec3 position = entity->GetGlobalPosition().xyz();
			InputManager* inputMgr = InputManager::Get();
			glm::ivec2 cursorDelta = inputMgr->GetCursorDelta();
			//glm::vec3 up = glm::rotate(entity->GetGlobalOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 up(0.0f, 1.0f, 0.0f);

			glm::vec3 strafeDirection = glm::normalize(glm::cross(direction, up));
			direction = glm::normalize(glm::mat3(glm::rotate(glm::radians(-cursorDelta.x * motion.m_RotationSpeed), up) * glm::rotate(glm::radians(-cursorDelta.y * motion.m_RotationSpeed), strafeDirection)) * direction);			

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

			entity->SetGlobalPosition(glm::vec4(position.x, position.y, position.z, 1.0f));
			entity->SetGlobalOrientation(glm::quat(0.0f, direction));
			entity->SetGlobalTransform(glm::translate(glm::mat4(1.0f), position) * glm::toMat4(entity->GetGlobalOrientation()));

			inputMgr->ResetCursorDelta();
		}
	}
}