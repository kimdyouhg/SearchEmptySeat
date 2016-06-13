#include<iostream>
#include<string>
#include<sstream>
#include<string.h>
#include"./mysqlite.h"

using namespace std;


bool MySqlite::OpenDB(char* input) {
	string str;
	int ret;

	str = input;
	str += ".db";

	ret = sqlite3_open_v2(str.c_str(), &aDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

	if(ret) {
		return false;
	}
	else {
		return true;
	}
}

bool MySqlite::CloseDB() {
	return (sqlite3_close(aDB) == SQLITE_OK);
}

bool MySqlite::CreateTable(int colNum, char* buffer) {
	sqlite3_stmt *stmt;
	string query("CREATE TABLE ");
	string str;
	string str2;
	string token;
	bool result = true;

	str2 = buffer;
	stringstream stream(str2);
	stream >> str;


	if(IsTableExist(str)) {
		cout << str <<" is exist!!"<< endl;
		return false;
	}

	str += " (id integer primary key autoincrement";
	query += str;

	string ft, st;
	while(true) {
		
		if(colNum>0){
			stream >> token;
			stream >> token;
			str = token + " ";
			stream >> token;
			str += token;
		}else{
			str = "!";
		}
		colNum--;

		if(str.compare("!") == 0) {
			break;
		}

		stringstream ss(str);
		ss >> ft;
		ss >> st;

		if(st.compare("text") != 0 && st.compare("integer") != 0) {
			cout << "(CreateTable) INVALID TYPE" << endl;
			continue;
		}

		str = ", " + ft + " " + st;
		query += str;
	}
	query += ")";

	sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	Begin();

	if(sqlite3_step(stmt) != SQLITE_DONE) {
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(aDB));
		result = false;
	}
	
	if(result == true){
		cout << "Table maded!!" << endl;
	}

	Commit();

	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);

	return result;
}

bool MySqlite::DeleteTable() {
	sqlite3_stmt *stmt;
	string query("DROP TABLE IF EXISTS ");
	string str;
	bool result = true;

	cout << "(DeleteTable) INPUT TABLE NAME : ";
	getline(cin, str);

	while(!IsTableExist(str)) {
		cout << "(DeleteTable) INPUT EXIST TABLE NAME : ";
		getline(cin, str);
	}
	query += str;

	sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	Begin();

	if(sqlite3_step(stmt) != SQLITE_DONE) {
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(aDB));
		result = false;
	}

	Commit();

	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);

	return result;
}


int MySqlite::FindData(char* buffer) {
	sqlite3_stmt *stmt;
	string query("SELECT * FROM ");
	string str;
	int ret = 0;
	int result;
	
	string temp;
	temp = buffer;
	stringstream stream(temp);

	stream >> str;

	while(!IsTableExist(str)) {
		cout << "(FindData) INPUT EXIST TABLE NAME : ";
		getline(cin, str);
	}

	query += str;
	query += " where ";

	stream >> str;
	stream >> str;


	query += str;
	query += "='";

	stream >>str;
	stream >>str;

	query += str;
	query += "'";

	ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(ret != SQLITE_OK) {
		cout << str << " IS INVALID" << endl;
		return -1;
	}

	ret = sqlite3_step(stmt);

	if (ret == SQLITE_ROW) {
		result = sqlite3_column_int(stmt, 0);
  	sqlite3_finalize(stmt);
		return result;
	}
	else {
  	sqlite3_finalize(stmt);
		return -1;
	}
}


bool MySqlite::UpsertData(char* buffer) {
	sqlite3_stmt *stmt;
	string query("SELECT * FROM ");
	string str;
	int index;
	int ret = 0;
	bool result = true;
	string texttemp[50];
	int times=0;

	deque<string> col_type;
	deque<string> col_name;
	
	string str2 = buffer;
	stringstream ss(str2);

	ss >> str;

	if(!IsTableExist(str)) {
		cout << "(UpsertData) NO TABLE EXIST" << endl;
		return false;
	}
	else {
		query += str;

		ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

		if(ret != SQLITE_OK) {
			cout << "(UpsertData) ANY DATA DOES NOT EXIST" << endl;
			return false;
		}

		query = "INSERT OR REPLACE INTO " + str + "(";

		int cols = sqlite3_column_count(stmt);
		string temp;

		for(int col = 1; col < cols; col++) {
			temp = sqlite3_column_name(stmt, col);
			query += sqlite3_column_name(stmt, col);

			col_name.push_back(temp);
			temp = sqlite3_column_decltype(stmt, col);
			col_type.push_back(temp);

			if(col + 1 != cols) {
				query += ", ";
			}
		}
		query += ") VALUES(";

		for(int col = 1; col < cols; col++) {
			query += "?";

			if(col + 1 != cols) {
				query += ", ";
			}
		}
		query += ")";

		ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

		if(ret != SQLITE_OK) {
			cout << query << " IS NOT VALID" << endl;
			return false;
		}

		sqlite3_reset(stmt);

		ss>> str;

		for(int col = 1; col < cols; col++) {
			if(col_type.front().compare("integer") == 0) {
				string inttemp;

				ss >> str;
				inttemp = str;
	
				sqlite3_bind_int(stmt, col, stoi(inttemp));

				if(ret != SQLITE_OK) {
					cout << "(UpsertData) BIND_INT ERROR" << endl;
				}
			}
			else {
				
				ss>> str;
				texttemp[times] = str;

				while(true){
					ss>>str;
					if(str[0] == '*' || str[0] == '#') break;
					texttemp[times] += " " + str;
				}

				sqlite3_bind_text(stmt, col, texttemp[times].c_str(), texttemp[times].size(), SQLITE_STATIC);
				times++;
				if(ret != SQLITE_OK) {
					cout << "(UpsertData) BIND_TEXT ERROR" << endl;
				}
			}
			col_type.pop_front();
			col_name.pop_front();
		}

		ret = sqlite3_step(stmt);

		if(ret != SQLITE_DONE) {
			cout << "(UpsertData) SQLITE STEP ERROR" << endl;
		}

		sqlite3_finalize(stmt);
	}
}


bool MySqlite::DeleteData(int rID,char* buffer) {
	sqlite3_stmt *stmt;
	string query("DELETE FROM ");
	string str;
	int ret = 0;

	str = buffer;

	if(!IsTableExist(str)) {
		cout << "(DeleteData) NO TABLE EXIST" << endl;
		getline(cin, str);
	}

	query += str;
	query += " WHERE id=" + to_string(rID);


	ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(ret != SQLITE_OK) {
		cout << query << " IS INVALID" << endl;
		sqlite3_finalize(stmt);
		return false;
	}

	ret = sqlite3_step(stmt);

	if(ret != SQLITE_DONE) {
		cout << "(DeleteData) SQLITE STEP ERROR" << endl;
	}

	sqlite3_finalize(stmt);

	return true;
}

string MySqlite::GetData(int rID, char* buffer) {
	sqlite3_stmt *stmt;
	string query("SELECT * FROM ");
	string str;
	int ret = 0;
	int cols;


//	cout << "(GetData) INPUT TABLE NAME : ";
//	getline(cin, str);
	str = buffer;

	if(!IsTableExist(str)) {
		cout << "(GetData) NO TABLE EXIST" << endl;
		getline(cin, str);
	}

	query += str;
	query += " WHERE id=" + to_string(rID);

	ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(ret != SQLITE_OK) {
		cout << query << " IS INVALID" << endl;
		return "NULL";
	}

	cols = sqlite3_column_count(stmt);

	string result;

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		for (int col = 1; col < cols; col++) {
			switch (sqlite3_column_type(stmt, col)) {
				case SQLITE_INTEGER:
					if(1 == col) {
						result = to_string(sqlite3_column_int(stmt, col));
					}
					else {
						result += " * " + to_string(sqlite3_column_int(stmt, col));
					}
					break;
				case SQLITE_TEXT:
					if(1 == col) {
						result = string(((const char*)sqlite3_column_text(stmt, col)));
					}
					else {
						result += " * " + string(((const char*)sqlite3_column_text(stmt, col)));
					}
					break;
				case SQLITE_NULL:
					if(1 == col) {
						result = "NULL";
					}
					else {
						result += " * NULL";
					}
					break;
			}
		}
		result += " #";
		sqlite3_finalize(stmt);

		return result;
	}
	else {
		sqlite3_finalize(stmt);
		return "NULL";
	}
}

deque<int> MySqlite::GetWantData(char* buffer) {
	sqlite3_stmt *stmt;
	deque<int> id_array;
	string query("SELECT * FROM ");
	string str;
	string str2;
	string result;
	int ret;
	int cols;
	int column;

	str2 = buffer;

	stringstream ss(str2);
	ss >> str;

	query += str;

//cout << "QUERY : " << query << endl;

	ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(ret != SQLITE_OK) {
		cout << str << " TABLE DATA NOT EXIST" << endl;
		return id_array;
	}

	cols = sqlite3_column_count(stmt);

	ss >> str;
	ss >> str;

	for(int col = 0; col < cols; col++) {
		//cout << sqlite3_column_name(stmt, col) << " " << endl;
		if(str.compare(string(((const char*)sqlite3_column_name(stmt, col)))) == 0){
			column = col;
			break;
		}
	}

//cout << "STR : " << str << "  column : " << column << endl;

	ss >> str;
	ss >> str;

//cout << "STR : " << str << endl;

	while(sqlite3_step(stmt) == SQLITE_ROW){
		switch (sqlite3_column_type(stmt, column)) {
			case SQLITE_INTEGER:
				result = to_string(sqlite3_column_int(stmt, column));

				if(str.compare(result) == 0){
					id_array.push_back(sqlite3_column_int(stmt, 0));
				}

				break;
			case SQLITE_TEXT:
				result = string(((const char*)sqlite3_column_text(stmt, column)));
//cout << "result : " << result << endl;

				if(str.compare(result) == 0){
//cout << sqlite3_column_int(stmt, 0) << endl;
					id_array.push_back(sqlite3_column_int(stmt, 0));
				}

				break;
			case SQLITE_NULL:

				break;
		}
	}

	sqlite3_finalize(stmt);

	return id_array;
}

void MySqlite::PrintTable(char* buffer) {
	sqlite3_stmt *stmt;
	string query("SELECT * FROM ");
	string str;
	int ret = 0;
	int cols;
	string str2;

	str2 = buffer;

	stringstream ss(str2);
	ss >> str;

	query += str;

	ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(ret != SQLITE_OK) {
		cout << str << " TABLE DATA NOT EXIST" << endl;
		return;
	}

	cols = sqlite3_column_count(stmt);

	for(int col = 0; col < cols; col++) {
		cout << " " << sqlite3_column_name(stmt, col);
	}
	cout << endl;

	while (true) {
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
      for (int col = 0; col < cols; col++) {
        switch (sqlite3_column_type(stmt, col)) {
          case SQLITE_INTEGER:
            cout << " " << sqlite3_column_int(stmt, col) << " ";
            break;
          case SQLITE_FLOAT:
            cout << " " << sqlite3_column_double(stmt, col) << " ";
            break;
          case SQLITE_TEXT:
            cout << " " << sqlite3_column_text(stmt, col) << " ";
            break;
          case SQLITE_NULL:
            cout << " " << "NULL" << " ";
            break;
        }
      }
      cout << endl;
    } else if (ret == SQLITE_DONE) {
      cout << "done" << endl;
      break;
    } else {
      cout << "ret:" << ret << endl;
      break;
    }
  }

  sqlite3_finalize(stmt);
}



void MySqlite::GetTable(char* buffer,char* dest){
	sqlite3_stmt *stmt;
	string query("SELECT * FROM ");
	string str;
	int ret = 0;
	int cols;
	string str2;
	string destination;
	string destBuf;
	str2 = buffer;

	stringstream ss(str2);
	ss >> str;

	query += str;

	ret = sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(ret != SQLITE_OK) {
		cout << str << " TABLE DATA NOT EXIST" << endl;
		return;
	}

	cols = sqlite3_column_count(stmt);

	for(int col = 0; col < cols; col++) {
		destBuf = sqlite3_column_name(stmt,col);
		destination += " " + destBuf;
	}
	destination += " *";

	while (true) {
    ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW) {
      for (int col = 0; col < cols; col++) {
        switch (sqlite3_column_type(stmt, col)) {
          case SQLITE_INTEGER:
						destBuf = to_string(sqlite3_column_int(stmt,col)) + " ";
						destination += " " + destBuf + " ";
            break;
          case SQLITE_FLOAT:
						destBuf = to_string(sqlite3_column_double(stmt,col));
						destination += " " + destBuf + " ";
            break;
          case SQLITE_TEXT:
						destBuf=string(((const char*)sqlite3_column_text(stmt, col)));
						destination += " " + destBuf + " ";
            break;
          case SQLITE_NULL:
            destination += " ";
						destination += "NULL";
						destination += " ";
            break;
        }
      }
    } else if (ret == SQLITE_DONE) {
      destination +="#";
			break;
    } else {
      cout << "ret:" << ret << endl;
      break;
    }
  }

	strcpy(dest,&destination[0]);
  sqlite3_finalize(stmt);

}



bool MySqlite::Begin() {
	sqlite3_stmt *stmt;
	string query("BEGIN");
	bool result = true;

	sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(sqlite3_step(stmt) != SQLITE_DONE) {
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(aDB));
		result = false;
	}

	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);

	return result;
}

bool MySqlite::Commit() {
	sqlite3_stmt *stmt;
	string query("COMMIT");
	bool result = true;

	sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(sqlite3_step(stmt) != SQLITE_DONE) {
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(aDB));
		result = false;
	}

	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);

	return result;
}

bool MySqlite::Rollback() {
	sqlite3_stmt *stmt;
	string query("ROLLBACK");
	bool result = true;

	sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(sqlite3_step(stmt) != SQLITE_DONE) {
		fwprintf(stderr, L"line %d: %s\n", __LINE__, sqlite3_errmsg16(aDB));
		result = false;
	}

	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);

	return result;
}

bool MySqlite::IsTableExist(string rTableName) {
	sqlite3_stmt *stmt;
	string query("SELECT name FROM sqlite_master WHERE type='table' AND name='");
	bool result;

	query += rTableName;
	query += "'";

	sqlite3_prepare_v2(aDB, query.c_str(), -1, &stmt, NULL);

	if(sqlite3_step(stmt) == SQLITE_ROW) {
		result = true;
	}
	else {
		result = false;
	}

	sqlite3_reset(stmt);
	sqlite3_finalize(stmt);

	return result;
}
