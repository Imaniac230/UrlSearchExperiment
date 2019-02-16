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
//#define SKIP_REPEATING_DOMAINS

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
		throw 1;

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
		throw 1;

	return aStr.substr(start_pos + 1, end_pos - start_pos - 1);
	}


int main()
	{
	std::cout << choose_url;

	size_t count_urls = 0, count_searches = 0, nof_empty_searches = 0, desired_line = 1;
	std::ofstream ofile;
	std::string url;
	std::cin >> url;
	std::string url_ref = url;

	std::cout << std::endl << start_url_message << url << std::endl << std::endl;

	while(1)
		{
		if(DownloadFile(url, stored_url_contents))
			for(std::string hlink : ExtractHyperlinks(stored_url_contents))
				{
				if(!hlink.compare(0, 4, "http") /*&& (hlink.find(' ') == std::string::npos)*/ && FindString(hlink, stored_urls))
					{
					ofile.open(stored_urls, std::ios::app);

#ifdef OPEN_PAGES_IN_BROWSER					
					BSTR b_hlink = _com_util::ConvertStringToBSTR(hlink.c_str());
					LPCWSTR lp_hlink = b_hlink;
					ShellExecute(NULL, L"open", BROWSER, lp_hlink, NULL, SW_SHOWNORMAL);
					SysFreeString(b_hlink);
#endif /* OPEN_PAGES_IN_BROWSER */

					if((url.find(SubString(2, 3, '/', hlink)) == std::string::npos) && (hlink.find("accounts.google.com") == std::string::npos))
						++count_urls;

					std::cout << hlink << std::endl;
					url = hlink;
					ofile << hlink << std::endl;
					ofile.close();
					break;
					}
				}
		++count_searches;
		//cout << "One search complete." << endl;
		if((count_searches - count_urls) > LIMIT_OF_EMPTY_SEARCHES_LOCAL)
			{
			if(count_urls == 0)
				++nof_empty_searches;
			url = url_ref;
			count_urls = count_searches = 0;
			}

		if(nof_empty_searches > LIMIT_OF_EMPTY_SEARCHES_GLOBAL)
			{
			std::cout << std::endl << LIMIT_OF_EMPTY_SEARCHES_GLOBAL << empty_searches_message;

			size_t current_line = 1;
			std::ifstream ifl;
			std::string newpage;

			ifl.open(stored_urls);
			while((current_line <= desired_line) && (getline(ifl, newpage)))
				{ ++current_line; }
			ifl.close();

			++desired_line;
			if((current_line) != desired_line)
				{
				std::cout << std::endl << choose_new_url;

				while(!_kbhit())
					{ Beep(500, 500); }

				desired_line = current_line;
				std::cin >> newpage;
				}

			url_ref = url = newpage;
			nof_empty_searches = count_urls = count_searches = 0;

			std::cout << std::endl << new_url_message << url << std::endl << std::endl;
			}
		}

	ofile.close();
	return 0;
	}