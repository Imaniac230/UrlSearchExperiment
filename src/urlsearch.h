#ifndef __URLSEARCH_H__
#define __URLSEARCH_H__

#include <cstdio>
#include <cstring>
#include <sstream>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <conio.h>
#include <iterator>
#include <set>
#include <regex>
#include <comutil.h>
#include <urlmon.h>

#pragma comment( lib, "urlmon" )

//#define OPEN_PAGES_IN_BROWSER
#define SKIP_REPEATING_DOMAINS

#define LIMIT_OF_EMPTY_SEARCHES_LOCAL 3
#define LIMIT_OF_EMPTY_SEARCHES_GLOBAL 3

constexpr auto stored_url_contents = "page.html";
constexpr auto stored_urls = "usedpages.txt";
constexpr auto choose_url = "Choose an initial starting url : ";
constexpr auto start_url_message = "Starting url: ";
constexpr auto empty_searches_message = "\a searches complete without changes.";
constexpr auto choose_new_url = "Exhausted all possibilities for initial starting url, choose another starting url: ";
constexpr auto new_url_message = "Using new url: ";

#ifdef OPEN_PAGES_IN_BROWSER
constexpr auto BROWSER = L"firefox";
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "kernel32.lib")
#endif /* OPEN_PAGES_IN_BROWSER */

enum TError
	{
	EBadParameters = 0,
	};

bool DownloadFile(const std::string aUrl, const std::string aPath)
	{
	return URLDownloadToFileA(nullptr, aUrl.c_str(), aPath.c_str(), 0, nullptr) == S_OK;
	}

std::string FileToString(const std::string aFileName)
	{
	std::ifstream file(aFileName);
	return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>{} };
	}

std::set<std::string> ExtractHyperlinks(const std::string aFileName)
	{
	static const std::regex hl_regex("<a href=\"(.*?)\"", std::regex_constants::icase);

	const std::string text = FileToString(aFileName);

	return { std::sregex_token_iterator(text.begin(), text.end(), hl_regex, 1),
			 std::sregex_token_iterator{} };
	}

size_t FindString(const std::string aStr, const char* aFileName)
	{
	if(!aFileName)
		throw EBadParameters;

	std::ifstream ifile;
	std::string linestr;
	ifile.open(aFileName);

	while(getline(ifile, linestr))
		{
		if(!linestr.compare(aStr))
			{
			ifile.close();
			return 0;
			}
		}

	ifile.close();
	return 1;
	}

std::string SubString(const size_t aStart, const size_t aEnd, const char aChar, std::string aStr)
	{
	if(aStart == aEnd)
		throw EBadParameters;

	bool start = false, end = false;
	size_t cur_pos = 0, occur = 0, start_pos = 0, end_pos = 0;
	for(char c : aStr)
		{
		if(c == aChar)
			++occur;
		if(!start && occur == aStart)
			{
			start = true;
			start_pos = cur_pos;
			}
		if(!end && occur == aEnd)
			{
			end = true;
			end_pos = cur_pos;
			}
		++cur_pos;
		}

	if(start_pos == 0 && end_pos == 0)
		return "";

	return aStr.substr(start_pos + 1, end_pos - start_pos - 1);
	}

#endif /* __URLSEARCH_H__ */
