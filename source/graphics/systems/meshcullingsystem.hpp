#pragma once
#include "core/ecs/system.hpp"

namespace Khan
{
	class Renderer;

	class MeshCullingSystem : public System
	{
	public:
		explicit MeshCullingSystem(Renderer& renderer);

		virtual void Update(float dt) override;

	private:
		Renderer& m_Renderer;
	};
}