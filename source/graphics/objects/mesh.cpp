#include "graphics/precomp.h"
#include "graphics/objects/mesh.hpp"

namespace Khan
{
	Mesh::Mesh()
		: m_ParentTransform(sizeof(glm::mat4))
	{
	}
}