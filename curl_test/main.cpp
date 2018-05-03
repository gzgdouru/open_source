#include "curl/include/curl/curl.h"
#include "../../cplusplus/StandardLibrary/public/headfile.hpp"

bool getUrl(const string &szUrl, const string &szFileName)
{
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();	// 初始化
	FILE *pfile = fopen(szFileName.c_str(), "w");
	if (!pfile)
	{
		cout << "open file: " << szFileName << " failed!" << endl;
		return false;
	}

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, szUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, pfile);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, pfile);
		res = curl_easy_perform(curl);	//执行
		if (res == CURLE_OK)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

int writeFunc(void *data, int size, int nmemb, string &szContent)
{
	long lsize = size * nmemb;
	string szTemp = string((char*)data, lsize);
	szContent += szTemp;
	return lsize;
}

bool getUrl2(const string &szUrl, string &szOut, string &szError)
{
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();	// 初始化
	bool bRes = false;

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, szUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &szOut);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

		res = curl_easy_perform(curl);
		szError = (res == CURLE_OK) ? "" : curl_easy_strerror(res);
		bRes = (res == CURLE_OK);
	}

	return bRes;
}

void getFileName(const string &szUrl, string &szFileName)
{
	size_t pos = szUrl.find_last_of("/");
	if (pos != string::npos)
	{
		szFileName = szUrl.substr(pos + 1);
	}
}

size_t write_file(void *buffer, size_t size, size_t nmemb, void *user_p)
{
	FILE *pfile = (FILE*)user_p;
	size_t totalSize = fwrite(buffer, size, nmemb, pfile);
	return totalSize;
}

bool getFile(const string& szUrl, string &szError)
{
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();
	bool bRes = true;
	string szFileName;
	getFileName(szUrl, szFileName);
	FILE *pfile = fopen(szFileName.c_str(), "ab+");
	if (!pfile)
	{
		szError = "open file " + szFileName + " failed!";
		bRes = false;
	}

	if (bRes && curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, szUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, pfile);
		res = curl_easy_perform(curl);

		szError = (res == CURLE_OK) ? "" : curl_easy_strerror(res);
		bRes = (res == CURLE_OK);
	}

	fclose(pfile);
	return bRes;
}

bool postUrl(const string &szUrl, const string &szParam, string &szRes, string &szError)
{
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);
	CURL* curl = curl_easy_init();
	bool bRes = false;

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, szUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, szParam);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &szRes);
		curl_easy_setopt(curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post  
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印调试信息 
		curl_easy_setopt(curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给write_data  
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location

		res = curl_easy_perform(curl);
		szError = (res == CURLE_OK) ? "" : curl_easy_strerror(res);
		bRes = (res == CURLE_OK);
	}

	return bRes;
}

int main(int argc, char *argv[])
{
	string szUrl = "www.baidu.com";
	string szFileName = "baidu.txt";

	/*if (getUrl(szUrl, szFileName))
	{
		cout << "geturl success!" << endl;
	}
	else
	{
		cout << "geturl: " << szUrl << " failed!!!" << endl;
	}*/

	string szOut;
	string szError;
	/*if (getUrl2(szUrl, szOut, szError))
	{
		cout << szOut << endl;
	}
	else
	{
		cout << "geturl: " << szUrl << " failed, error: " << szError << endl;
	}*/

	szUrl = "http://localhost:9000/123.png";
	/*if (getFile(szUrl, szError))
	{
		cout << "get file success..." << endl;
	}
	else
	{
		cout << "get file failed, error: " << szError << endl;
	}*/

	szUrl = "http://saptest1.dyajb.com:9007/upgrade/login";
	string szParam = "_1=ouru&_2=5201314ouru&_3=登陆";
	if (postUrl(szUrl, szParam, szOut, szError))
	{
		cout << "postUrl suceess." << endl;
		cout << szOut << endl;
	}
	else
	{
		cout << "postUrl failed, error: " << szError << endl;
	}
	 
	getchar();
	return 0;
}