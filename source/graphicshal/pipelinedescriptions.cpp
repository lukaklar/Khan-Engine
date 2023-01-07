#include "graphicshal/precomp.h"
#include "graphicshal/pipelinedescriptions.hpp"

namespace Khan
{
	RenderPipelineState::~RenderPipelineState()
	{
	}
}

std::size_t std::hash<Khan::GraphicsPipelineDescription>::operator()(const Khan::GraphicsPipelineDescription& key) const
{
	const std::size_t* dataToHash = reinterpret_cast<const std::size_t*>(&key);
	const std::size_t elementsCount = sizeof(Khan::GraphicsPipelineDescription) / sizeof(std::size_t);

	std::size_t result = 17;
	for (std::size_t i = 0; i < elementsCount; ++i)
	{
		result = result * 31 + std::hash<std::size_t>()(dataToHash[i]);
	}

	return result;
}

std::size_t std::hash<Khan::ComputePipelineDescription>::operator()(const Khan::ComputePipelineDescription& key) const
{
	return reinterpret_cast<std::size_t>(key.m_ComputeShader);
}