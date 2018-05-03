#include <iostream>
#include <string>
#include <cstdio>
#include "openssl/md5.h"
#include "../../cplusplus/StandardLibrary/public/headfile.hpp"
using namespace std;

void test1()
{
	MD5_CTX ctx;
	unsigned char ottmd[16] = {0};
	int i = 0;
	string szStr = "hello";

	MD5_Init(&ctx);
	MD5_Update(&ctx, szStr.c_str(), szStr.length());
	MD5_Final(ottmd, &ctx);

	for (i = 0; i < 16; ++i)
	{
		printf("%02X", ottmd[i]);
	}
	cout << endl;

	string szMd5;
	for_each(begin(ottmd), end(ottmd), [&szMd5](unsigned char &ucOtt) {
		char cTemp[3] = { 0 };
		sprintf(cTemp, "%02X", (int)ucOtt);
		szMd5 += cTemp;
	});
	cout << szMd5 << endl;
}

int main(int argc, char *argv[])
{
	test1();
	return 0;
}
