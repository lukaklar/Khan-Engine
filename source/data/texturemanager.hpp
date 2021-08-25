#pragma once
#include "core/singleton.h"

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

		std::vector<Texture*> m_Textures;
		std::unordered_map<std::string, TextureView*> m_TextureViewMap;
	};
}