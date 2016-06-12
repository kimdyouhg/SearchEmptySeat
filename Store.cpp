#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;


class Store
{
	public:

		void StartStore(void);
		void WorkStore(void);
		void EndStore(void);

	private:

		int aClient;
		int aPortNum;
		int aBufSize;
		char *aBuffer;
		char *aIp;

		bool aLogIn(void);
		void aPrintMenu(void);

		void aGetStoreTableInfo(void);
		void aGetStoreMenuInfo(void);
		void aGetStoreReview(void);
		void aClearMenuAndReset(void);
		void aClearTableAndReset(void);
		void aClearReview(void);
		void aResetPassword(void);
		void aFullTable(void);
		void aEmptyTable(void);

};


static Store store;


int main(int argc, char* argv[]){
	
	store.StartStore();
	store.WorkStore();
	store.EndStore();

	return 0;
}


void Store::StartStore(void){
	
	aPortNum = 1500;
	aBufSize = 8192;
	aBuffer = new char[aBufSize];
	
	aIp = new char[15];

	strcpy(aIp,"127.0.0.1");

	string str;

	struct sockaddr_in server_addr;

	//init socket

	aClient = socket(AF_INET, SOCK_STREAM, 0);

	if(aClient < 0){
		cout << "Error creating socket..."<< endl;
		exit(1);
	}
	
	cout << "Clinet Socket created"<< endl;

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(aPortNum);
	
	memset(&(server_addr.sin_zero),0,8);
	server_addr.sin_addr.s_addr = inet_addr(aIp);
	
	//connecting socket server

	if(connect(aClient, (struct sockaddr*)&server_addr, sizeof(server_addr))==0){
		cout << "Connecting to server..."<< endl;
	}

	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

}


void Store::WorkStore(void){

	string str;

	if(!aLogIn()){
		return;
	}

	while(true){
		aPrintMenu();
		getline(cin,str);
	
		if(str == "1"){
			
			aGetStoreTableInfo();

		}else if(str == "2"){
			
			aGetStoreMenuInfo();

		}else if(str == "3"){

			aGetStoreReview();

		}else if(str == "4"){

			aClearMenuAndReset();
	
		}else if(str == "5"){

			aClearTableAndReset();
				
		}else if(str == "6"){

			aClearReview();
						
		}else if(str == "7"){	
			
			aResetPassword();

		}else if(str == "8"){
			
			aFullTable();

		}else if(str == "9"){
			
			aEmptyTable();

		}else if(str == "10"){

			aBuffer[0] = '!';
			send(aClient,aBuffer,aBufSize,0);
			break;

		}else{

			cout << "You put invalid input!!!" << endl;
			continue;
		
		}
	}
	

}


void Store::EndStore(void){

	
	cout << "connection terminated..." << endl;
	cout << "Goodbye" << endl;

	delete[] aBuffer;
	delete[] aIp;

	close(aClient);

}


bool Store::aLogIn(void){
	
	string str;

	cout << "LogIn start!!"<< endl;

	while(true){
		strcpy(aBuffer, "store");
		send(aClient,aBuffer,aBufSize,0);

		recv(aClient,aBuffer,aBufSize,0);
		str = aBuffer;

		if(str == "You are store..."){
			cout << "Store Mode valid..."<< endl;
			break;
		}
	}

	while(true){
//		cout << "Storename * password #"<< endl;
		cout << "1. Log in Store"<< endl;
		cout << "2. Register Store"<< endl;
		cout << "3. Finish"<< endl;

		getline(cin,str);

		if(str == "1"){
			strcpy(aBuffer,"login");
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			
			//log in session
			string temp;
			string temp2;
			cout << aBuffer << endl;

			cout <<"your Store Name = ";
			getline(cin,temp);
			temp2 = temp;
			cout <<"your Password = ";
			getline(cin,temp);
			temp2 += " * " + temp + " #";

			strcpy(aBuffer,&temp2[0]);
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			str = aBuffer;

			if(str == "You are store member"){
				cout << "You are store member" << endl;
				break;
			}else{
				cout << str<< endl;
			}
		}else if(str == "2"){
			strcpy(aBuffer,"register");
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			cout << aBuffer << endl;

			string temp;
			cout << "name = ";
			getline(cin,temp);
			str = temp;
			cout << "Password = ";
			getline(cin,temp);
			str += " * " + temp + " #";
			
			strcpy(aBuffer,&str[0]);
			send(aClient,aBuffer,aBufSize,0);
			
			recv(aClient,aBuffer,aBufSize,0);
			cout <<aBuffer << endl;
			str = aBuffer;

			//Additional infomation
			if(str == "success"){
				while(true){
					cout << "What is your store's genre? "<< endl;
					cout << "1.Korean" << endl;
					cout << "2.American" << endl;
					cout << "3.Chinese" << endl;
					cout << "4.Japanese" << endl;
					cout << "Any Key the others" << endl;
					getline(cin,temp);
					
					if(temp == "1"){
						str = "Korean";
					}else if(temp == "2"){
						str = "American";
					}else if(temp == "3"){
						str = "Chinese";
					}else if(temp == "4"){
						str = "Japanese";
					}else{
						str = "other";
					}

					cout << "How many tables do you have? ";
					getline(cin,temp);
					int tem_int = atoi(&temp[0]);
					int tem_index=1;
					str += " * tablenum " + temp + " /";

					while(tem_int>0){
						cout << "How many people table"<<tem_index<<" can take? " ;
						getline(cin, temp);
						str += " " + temp + " 0";
	
						tem_int--;
						tem_index++;
					}

					cout << "How many menu does store have? ";
					getline(cin, temp);
					tem_int = atoi(&temp[0]);
					tem_index=1;
					str += " * menunum " + temp + " /";

					while(tem_int>0){
						cout << "num" << tem_index<<" menu's name? ";
						getline(cin,temp);
						str += " " +temp;
					
						cout <<"This menu's price ? ";
						getline(cin,temp);
						str += " " +  temp;
						tem_int--;
						tem_index++;
					}
					str += " #";
					strcpy(aBuffer,&str[0]);
					send(aClient,aBuffer,aBufSize,0);

					recv(aClient,aBuffer,aBufSize,0);
					cout << aBuffer << endl;
					str = aBuffer;
					if(str == "Register success"){
						break;
					}
				}
			}
		}else{
			strcpy(aBuffer,"finish");
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			cout << aBuffer <<endl;

			return false;
		}
	}
	return true;
}


void Store::aPrintMenu(void){
	
	cout <<"-----------------------------------"<<endl;
	cout <<"1  Get Store Table Information."<< endl;
	cout <<"2  Get Store Menu Information."<< endl;
	cout <<"3. Get Store Review."<<endl;
	cout <<"4. Clear Menu and Reset."<<endl;
	cout <<"5. Clear Table and Reset."<<endl;
	cout <<"6. Clear Review."<<endl;
	cout <<"7. Reset Password" <<endl;
	cout <<"8. Full Table"<< endl;
	cout <<"9. Empty Table"<<endl;
	cout <<"10.Log out" <<endl;

}


void Store::aGetStoreTableInfo(void){
	
	string str;

	strcpy(aBuffer,"s1");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	recv(aClient,aBuffer,aBufSize,0);

	string str2;
	string temp;
	string temp2;
	int index =1;
	int times;
	str2 = aBuffer;
	stringstream ss(str2);
	
	ss >> temp; //name
	ss >> temp; //*
	ss >> temp; //table num;
	times = atoi(&temp[0]);
	cout << "Number of tables is "<<temp<< endl;

	ss >> temp; // /	
	while(times >0){
		ss >> temp; //people num
		ss >> temp2; //exist
	
		cout << "table" << index << "'s capacity is "<<temp<< " : ";
		if(temp2 == "0"){
			cout <<"empty"<< endl;
		}else{
			cout <<"full"<< endl;
		}

		times--;
		index++;
	}
	
	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aGetStoreMenuInfo(void){

	string str;
	
	strcpy(aBuffer,"s2");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	recv(aClient, aBuffer,aBufSize,0);

	string str2;
	string temp;
	string temp2;
	int index =1;
	int times;
	str2 = aBuffer;
	stringstream ss(str2);

	ss >> temp; //name;
	ss >> temp; //*;
	ss >> temp; //table num;
	
	times = atoi(&temp[0]);
	ss >> temp; // /
	while(times>0){
		ss >> temp;
		ss >> temp;
		times--;
	}
	ss >> temp; // *
	ss >> temp; //menu num;
	times = atoi(&temp[0]);
	ss >> temp; // /

	cout <<"index\tname\tprice"<<endl;
	while(times>0){
		ss >> temp;
		ss >> temp2;
		
		cout << index << ".\t" << temp << "\t"<< temp2 << endl;

		times--;
		index++;
	}

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aGetStoreReview(void){
	
	string str;

	strcpy(aBuffer,"s3");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	recv(aClient,aBuffer,aBufSize,0);
	
	string str2;
	string temp;
	string temp2;
	int index =1;
	int times;
	str2 = aBuffer;
	stringstream ss(str2);

	ss >> temp; //name
	ss >> temp; // *
	ss >> temp; // point

	cout << endl;
	cout << " - Point : " << stoi(temp) << " -"<< endl;

	ss >> temp; // *
	ss >> temp; // cnt
	ss >> temp; // *

	while(ss>>temp){
		if(temp[0] == '#'){
			cout<< endl;
			break;
		}else if(temp[0] == '/'){
			cout<< endl;
		}else{
			cout << " " << temp;
		}
	}


	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aClearMenuAndReset(void){

	string str;

	strcpy(aBuffer,"s4");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	recv(aClient,aBuffer,aBufSize,0);
	
	string str2;
	string temp;
	string temp2;
	string update;
	int index =1;
	int times;
	str2 = aBuffer;
	stringstream ss(str2);

	ss >> update; // name
	ss >> temp;	//*
	update += " " + temp;
	ss >> temp; //table num;
	update += " " + temp;
	
	times = atoi(&temp[0]);

	ss >> temp; // /
	update += " " + temp;
	while(times>0){
		ss >> temp;
		update += " " + temp;
		ss >> temp;
		update += " " + temp;
		times--;
	}

	update += " *";

	cout << "How many menu store have? = ";
	getline(cin , temp);
	update += " " + temp + " /";

	times = atoi(&temp[0]);

	while(times >0){
		cout << "menu" <<index<<"'s name = ";
		getline(cin,temp);
		update += " " + temp;
		cout << "price = ";
		getline(cin,temp);
		update += " " + temp;

		times--;
		index++;
	}
	update += " #";

	strcpy(aBuffer, &update[0]);
	send(aClient,aBuffer,aBufSize,0);

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aClearTableAndReset(void){

	string str;

	strcpy(aBuffer,"s5");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	recv(aClient,aBuffer,aBufSize,0);
	
	string str2;
	string temp;
	string temp2;
	string update;
	int index =1;
	int times;
	str2 = aBuffer;
	stringstream ss(str2);

	ss >> update; // name
	ss >> temp;	//*
	update += " " + temp;
	ss >> temp; //table num;
	
	times = atoi(&temp[0]);

	ss >> temp; // /
	
	while(times>0){
		ss >> temp;
		ss >> temp;
		times--;
	}

	cout << "How many table store have ? ";
	getline(cin,temp);

	update += " " + temp;
	
	times = atoi(&temp[0]);
	
	update += " /";

	while(times>0){
		cout << "table"<<index<<"'s capacity = ";
		getline(cin,temp);
		update += " " + temp + " 0";

		times --;
		index ++;
	}


	ss >> temp; // *;
	update += " " + temp;

	ss >> temp; // menu num
	update += " " + temp;

	times = atoi(&temp[0]);

	ss >> temp; // /
	update += " " + temp;


	while(times >0){
		ss >> temp;
		update += " " + temp;
		ss >> temp;
		update += " " + temp;

		times--;
	}
	update += " #";

	strcpy(aBuffer, &update[0]);
	send(aClient,aBuffer,aBufSize,0);


	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aClearReview(void){
	
	string str;

	strcpy(aBuffer,"s6");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aResetPassword(void){

	string str;
	string str2;

	strcpy(aBuffer,"s7");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout << "New password = ";
	getline(cin,str);
	strcpy(aBuffer,&str[0]);
	send(aClient,aBuffer,aBufSize,0);

	recv(aClient,aBuffer,aBufSize,0);
	
	str2 = aBuffer;

	if(str == str2){
		cout <<"Password Updated!!"<<endl;
	}else{
		cout <<"Faital Error!!"<< endl;
	}

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aFullTable(void){

	string str;

	strcpy(aBuffer,"s8");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout <<"Which table full ? ";
	getline(cin,str);
	
	strcpy(aBuffer,&str[0]);
	send(aClient,aBuffer,aBufSize,0);

	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Store::aEmptyTable(void){

	string str;

	strcpy(aBuffer,"s9");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout <<"Which table empty ? ";
	getline(cin,str);
	
	strcpy(aBuffer,&str[0]);
	send(aClient,aBuffer,aBufSize,0);

	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;


	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


