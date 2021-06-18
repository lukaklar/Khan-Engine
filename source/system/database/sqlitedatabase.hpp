#include "system/precomp.h"

struct sqlite3;

namespace Khan
{
	class SQLiteDatabase
	{
	public:
		~SQLiteDatabase();

		bool Open(const std::string& databaseName);
		bool Close();
		bool Execute(const char* sql);
		bool Execute(const char* sql, int (*callback)(void*, int, char**, char**));
		std::string GetLastError() const;

		inline const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;
		sqlite3* m_Database = nullptr;
	};
}