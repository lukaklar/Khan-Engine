#include "data/precomp.h"
#include "core/string.h"

struct sqlite3;

namespace Khan
{
	class SQLiteDatabase
	{
	public:
		~SQLiteDatabase();

		bool Open(const char* databaseName);
		bool Close();
		bool ExecuteSQL(const char* sql);
		bool ExecuteSQL(const char* sql, int (*callback)(void*, int, char**, char**));
		String GetLastError() const;

		inline const String& GetName() const { return m_Name; }

	private:
		String m_Name;
		sqlite3* m_Database = nullptr;
	};
}