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


int main()
	{
	std::cout << "Choose an initial starting url: ";

	size_t count_urls = 0, count_searches = 0, nof_empty_searches = 0, desired_line = 1;
	std::ofstream ofile;
	std::string url;
	std::cin >> url;
	std::string url_ref = url;
	const std::string path = "page.html";

	std::cout << std::endl << "Starting url: " << url << std::endl << std::endl;

	while(1)
		{
		ofile.open("usedpages.txt", std::ios::app);

		if(DownloadFile(url, path))
			for(std::string hlink : ExtractHyperlinks(path))
				{
				if(!hlink.compare(0, 4, "http") && (hlink.find(' ') == std::string::npos) && FindString(hlink, "usedpages.txt"))
					{

#ifdef OPEN_PAGES_IN_BROWSER					
					BSTR b_hlink = _com_util::ConvertStringToBSTR(hlink.c_str());
					LPCWSTR lp_hlink = b_hlink;
					ShellExecute(NULL, L"open", BROWSER, lp_hlink, NULL, SW_SHOWNORMAL);
					SysFreeString(b_hlink);
#endif /* OPEN_PAGES_IN_BROWSER */

					std::cout << hlink << '\n';
					url = hlink;
					ofile << hlink << std::endl;
					++count_urls;
					break;
					}
				}
		++count_searches;
		//cout << "One search complete." << endl;
		if((count_searches - count_urls) > 3)
			{
			if(count_urls == 0)
				++nof_empty_searches;
			url = url_ref;
			count_urls = count_searches = 0;
			}

		if(nof_empty_searches > 3)
			{
			std::cout << std::endl << "\aThree searches complete without changes.";

			size_t current_line = 1;
			std::ifstream ifl;
			std::string newpage;

			ifl.open("usedpages.txt");
			while((current_line <= desired_line) && (getline(ifl, newpage)))
				{
				++current_line;
				}
			ifl.close();

			++desired_line;
			if((current_line) != desired_line)
				{
				std::cout << std::endl << "Exhausted all possibilities for initial starting url, choose another starting url: ";

				DWORD time_end = (DWORD)0;
				bool key_hit = false;

				for(;;)
					{
					Beep(5000, 1000);
					time_end = GetTickCount() + 500;
					while(time_end > GetTickCount())
						{
						if(_kbhit())
							{
							key_hit = true;
							break;
							}
						}
					if(key_hit)
						break;
					}

				desired_line = current_line;
				std::cin >> newpage;
				}

			url_ref = url = newpage;
			nof_empty_searches = count_urls = count_searches = 0;

			std::cout << std::endl << "Using new url: " << url << std::endl << std::endl;
			}

		ofile.close();
		}

	ofile.close();
	return 0;
	}