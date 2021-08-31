#pragma once
#include "core/singleton.h"
#include "data/database/sqlitedatabase.hpp"

namespace Khan
{
	struct Mesh;
	class World;

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

		void CreateTestPlayground(World* world);

		//SQLiteDatabase m_Database;

		std::map<uint32_t, Mesh*> m_Meshes;

		static constexpr char* ms_AssetPath = "..\\..\\assets\\";
	};
}