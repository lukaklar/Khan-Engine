#pragma once
#include "core/containers.h"
#include "core/singleton.h"
#include "core/string.h"

namespace Khan
{
	class Texture;
	class TextureView;

	class TextureManager : public Singleton<TextureManager>
	{
		friend class Singleton<TextureManager>;
	public:
		TextureView* LoadTexture(const char* fileName);

	private:
		TextureManager();
		~TextureManager();

		TArray<Texture*> m_Textures;
		THashMap<String, TextureView*> m_TextureViewMap;
	};
}