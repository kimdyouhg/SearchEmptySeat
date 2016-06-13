#ifndef __MYSQLITE_H__
#define __MYSQLITE_H__

#include<string>
#include<deque>
#include<sqlite3.h>

using namespace std;

class MySqlite {
private:
	sqlite3 *aDB;

public:
	bool OpenDB(char* input);
	bool CloseDB();
	bool CreateTable(int colNum, char* buffer);
	bool DeleteTable();
	int FindData(char* buffer);
	bool UpsertData(char* buffer);
	bool DeleteData(int rID,char* buffer);
	bool DeleteData();
	string GetData(int rID,char* buffer);
	deque<int> GetWantData(char* buffer);
	void PrintTable(char* buffer);
	void GetTable(char* buffer,char* dest);

private:
	bool IsTableExist(string rTableName);
	bool Begin();
	bool Commit();
	bool Rollback();
};

#endif
