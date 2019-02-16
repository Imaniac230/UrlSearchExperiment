#include "urlsearch.h"


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

#ifdef SKIP_REPEATING_DOMAINS
					if((url.find(SubString(2, 3, '/', hlink)) == std::string::npos) && (hlink.find("accounts.google.com") == std::string::npos))
#endif /* SKIP_REPEATING_DOMAINS */
						++count_urls;

					std::cout << hlink << std::endl;
					url = hlink;
					ofile << hlink << std::endl;
					ofile.close();
					break;
					}
				}

		++count_searches;
		if((count_searches - count_urls) >= LIMIT_OF_EMPTY_SEARCHES_LOCAL)
			{
			if(count_urls == 0)
				++nof_empty_searches;
			url = url_ref;
			count_urls = count_searches = 0;
			}

		if(nof_empty_searches >= LIMIT_OF_EMPTY_SEARCHES_GLOBAL)
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