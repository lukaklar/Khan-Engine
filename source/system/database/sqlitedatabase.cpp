#include "system/precomp.h"
#include "system/database/sqlitedatabase.hpp"
#include "system/database/sqlite/sqlite3.h"
#include "system/assert.h"

namespace Khan
{
	SQLiteDatabase::~SQLiteDatabase()
	{
		KH_ASSERT(m_Database == nullptr, "Database connection wasn't closed.");
	}

	bool SQLiteDatabase::Open(const std::string& databaseName)
	{
		KH_ASSERT(m_Database == nullptr, "Database already opened.");
		if (sqlite3_open(databaseName.c_str(), &m_Database))
		{
			//m_LastError = sqlite3_errmsg(m_Database);
			Close();
			return false;
		}

		return true;
	}

	bool SQLiteDatabase::Close()
	{
		KH_ASSERT(m_Database != nullptr, "Database not opened.");
		bool result = true;
		if (sqlite3_close(m_Database))
		{
			//m_LastError = sqlite3_errmsg(m_Database);
			result = false;
		}

		m_Database = nullptr;
		return result;
	}

	bool SQLiteDatabase::Execute(const char* sql)
	{
		return sqlite3_exec(m_Database, sql, nullptr, nullptr, nullptr) == SQLITE_OK;
	}

	bool SQLiteDatabase::Execute(const char* sql, int (*callback)(void*, int, char**, char**))
	{
		return sqlite3_exec(m_Database, sql, callback, nullptr, nullptr) == SQLITE_OK;
	}

	std::string SQLiteDatabase::GetLastError() const
	{
		return sqlite3_errmsg(m_Database);
	}
}