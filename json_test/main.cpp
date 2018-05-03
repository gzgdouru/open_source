#include "../../cplusplus/StandardLibrary/public/headfile.hpp"
#include "json/json.h"
#include <assert.h>

void test1()
{
	ifstream ifs("testJson.txt");
	assert(ifs.is_open());

	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(ifs, root, false))
	{
		cout << "json parse failed!" << endl;
		return;
	}

	string szName = root["name"].asString();
	int iage = root["age"].asInt();
	cout << "name: " << szName << endl;
	cout << "age: " << iage << endl;
}

void test2()
{
	Json::Value root;
	root["type"] = 0;
	root["name"] = "Å·Èå";
	root["passwd"] = "123456";
	string szStr = root.toStyledString();
	cout << szStr << endl;

	Json::Reader reader;
	Json::Value value;
	reader.parse(szStr, value);
	cout << value["type"] << endl;
	cout << value["name"] << endl;
	cout << value["passwd"] << endl;
}

void test3()
{
	Json::Reader reader;
	Json::Value root;

	ifstream ifs("testJson2.txt", ios::binary);
	assert(ifs.is_open());
	if (reader.parse(ifs, root))
	{
		string szCode;
		if (!root["files"].isNull())
			szCode = root["uploadid"].asString();

		szCode = root.get("uploadid", NULL).asString();
		
		int iFilesSize = root.get("files", 0).size();
		for (int i = 0; i < iFilesSize; ++i)
		{
			cout << "files" << i + 1 << ":" << endl;
			Json::Value Image = root["files"][i]["images"];
			int iImageSize = Image.size();
			for (int j = 0; j < iImageSize; ++j)
			{
				cout << "image" << j + 1 << ":" << endl;
				cout << Image[j] << endl;
			}
		}
	}
	else
	{
		cout << "json parse failed!" << endl;
	}
}

void test4()
{
	Json::Value arrayObj;
	Json::Value newItem, newItem1;
	newItem["date"] = "2018-01-10";
	newItem1["time"] = "14:55:00";
	arrayObj.append(newItem);
	arrayObj.append(newItem1);
	//cout << arrayObj.toStyledString() << endl;

	ifstream ifs("testJson2.txt", ios::binary);
	assert(ifs.is_open());
	Json::Reader reader;
	Json::Value root;
	reader.parse(ifs, root);

	int iFilesSize = root["files"].size();
	for (int i = 0; i < iFilesSize; ++i)
	{
		root["files"][i]["datetime"] = arrayObj;
	}
	cout << root.toStyledString() << endl;

	/*Json::FastWriter writer;
	string szOut = writer.write(arrayObj);
	cout << szOut << endl;*/
}

class CPerson
{
	friend ostream& operator<<(ostream&, const CPerson&);
public:
	CPerson() {}
	CPerson(const string& szName, int iAge, const string& szSex) :
		m_szName(szName), m_iAge(iAge), m_szSex(szSex)
	{
			
	}

	virtual ~CPerson() {}

	string serialization()
	{
		Json::Value values;
		values["name"] = m_szName;
		values["age"] = m_iAge;
		values["sex"] = m_szSex;
		
		return values.toStyledString();
	}

	bool deserialization(const string& szStr)
	{
		bool bRes = false;
		Json::Reader reader;
		Json::Value root;

		if (readerng.parse(szStr, root))
		{
			m_szName = root.get("name", NULL).asString();
			m_iAge = root.get("age", NULL).asInt();
			m_szSex = root.get("sex", NULL).asString();
			bRes = true;
		}

		return bRes;
	}

private:
	string m_szName;
	int m_iAge;
	string m_szSex;
};

ostream& operator<<(ostream& os, const CPerson& person)
{
	return os << "name:" << person.m_szName << " age:" << person.m_iAge << " sex:" << person.m_szSex << endl;
}

void test5()
{
	CPerson person("ouru", 25, "man");
	cout << person << endl;

	string szPerson = person.serialization();
	cout << szPerson << endl;

	CPerson resPerson;
	resPerson.deserialization(szPerson);
	cout << resPerson << endl;
}

int main(int argc, char *argv[])
{
	test5();
	return 0;
}
