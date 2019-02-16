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
constexpr auto choose_url = "Choose an initial starting url or type \"exit\" to end: ";
constexpr auto start_url_message = "Starting url: ";
constexpr auto empty_searches_message = "\a searches complete without changes.";
constexpr auto choose_new_url = "Exhausted all possibilities for initial starting url, choose another starting url or type \"exit\" to end: ";
constexpr auto new_url_message = "Using new url: ";
constexpr auto bad_url = "Could not download url contents.";

#ifdef OPEN_PAGES_IN_BROWSER
constexpr auto BROWSER = L"firefox";
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "kernel32.lib")
#endif /* OPEN_PAGES_IN_BROWSER */

enum TError
	{
	EBadParameters = 0,
	};

bool DownloadFile(const std::string aUrl, const std::string aPath);

std::string FileToString(const std::string aFileName);

std::set<std::string> ExtractHyperlinks(const std::string aFileName);

size_t FindString(const std::string aStr, const char* aFileName);

std::string SubString(const size_t aStart, const size_t aEnd, const char aChar, std::string aStr);

#endif /* __URLSEARCH_H__ */
