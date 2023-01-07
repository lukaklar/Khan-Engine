#include "graphicshal/precomp.h"
#include "graphicshal/bufferview.hpp"

std::size_t std::hash<Khan::BufferViewDesc>::operator()(const Khan::BufferViewDesc& key) const
{
	const std::size_t* dataToHash = reinterpret_cast<const std::size_t*>(&key);
	const std::size_t elementsCount = sizeof(Khan::BufferViewDesc) / sizeof(std::size_t);

	std::size_t result = 17;
	for (std::size_t i = 0; i < elementsCount; ++i)
	{
		result = result * 31 + std::hash<std::size_t>()(dataToHash[i]);
	}

	return result;
}