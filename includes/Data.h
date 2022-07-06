#pragma once
#include <string>
#include <vector>


struct CookieData
{
	std::string hostKey;
	std::string name;
	std::string value;
	std::string path;
	std::string expireUTC;
};

struct userData
{
	std::string url;
	std::string username;
	std::string password;
};

struct historyData
{
	std::string url;
	std::string title;
	std::string visit_count;
	std::string last_visit_time;
};
struct downloadsData
{
	std::string target_path;
	std::string tab_url;
	std::string total_bytes;
	std::string start_time;
	std::string end_time;
};