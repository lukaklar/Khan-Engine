#pragma once
#include "core/singleton.h"
#include "data/database/sqlitedatabase.hpp"

namespace Khan
{
	class World;
	class TextureView;

	class DataManager : public Singleton<DataManager>
	{
		friend class Singleton<DataManager>;
	public:
		World* LoadWorldFromFile(const char* fileName);
		//World* LoadWorldFromDB(const char* name);
		//void SaveWorldToDB(const World* world);

	private:
		DataManager();
		~DataManager();

		//TextureView* LoadTextureFromFile(const char* fileName);

		//SQLiteDatabase m_Database;

		static constexpr char* ms_AssetPath = "..\\..\\assets\\";
	};
}