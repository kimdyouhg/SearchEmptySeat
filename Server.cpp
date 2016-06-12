#include <iostream>
#include <sstream>
#include <vector>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "./mysqlite.h"

using namespace std;

class Manager
{
	public:

		void ChooseMode(int client);
		bool LogIn(int client, Manager& manager);
		bool GetisClient(void) {return isClient;}
		bool GetisMember(void) {return isMember;}
		void SetaName(char* buffer);
		string GetaName(void) { return aName;}

	private:

		bool isClient;
		bool isMember;
		string aName;
};

class Server
{
	public:

		void StartServer(void);
		void EndServer(void);
		bool RegisterClient(int client);
		bool RegisterStore(int client);
		bool LogInClient(int client, Manager &manager);
		bool LogInStore(int client, Manager &manager);

		void StoreGetStoreTableInfo(int client, Manager manager);
		void StoreGetStoreMenuInfo(int client, Manager manager);
		void StoreGetStoreReview(int client, Manager manager);
		void StoreClearMenuAndReset(int client, Manager manager);
		void StoreClearTableAndReset(int client, Manager manager);
		void StoreClearReview(int client, Manager manager);
		void StoreResetPassword(int client, Manager manager);
		void StoreFullTable(int client, Manager manager);
		void StoreEmptyTable(int client, Manager manager);

		void ClientShowAllStore(int client,Manager manager);
		void ClientGetStoreTableInfo(int client, Manager manager);
		void ClientGetStoreMenuInfo(int client, Manager manager);
		void ClientGetStoreReview(int client, Manager manager);
		void ClientSetStoreReview(int client, Manager manager);
		void ClientRecommendStore(int client, Manager manager);
		void ClientResetPassword(int client, Manager manager);

	private:

		pthread_t aThread;
		pthread_attr_t aAttr;

		pthread_mutex_t aClientAccount_lock;
		pthread_mutex_t aClientInfo_lock;
		pthread_mutex_t aStoreAccount_lock;
		pthread_mutex_t aStoreGenre_lock;
		pthread_mutex_t aStoreInfo_lock;

		MySqlite aClientAccount;
		MySqlite aClientInfo;

		MySqlite aStoreAccount;
		MySqlite aStoreGenre;
		MySqlite aStoreInfo;

};


void* ServerWaitClient(void *arg);
void* ServerWork(void *arg);

static Server server;


int main(int argc, char* argv[]){
	
	server.StartServer();

	while(true){
		string input;

		cout << "If you push 'end' server will be terminated!!" << endl;
		cin >> input;

		if(input == "end"){
			cout << "Server is terminated!!"<< endl;
			break;
		}
	}

	server.EndServer();

	return 0;
}


void* ServerWaitClient(void *arg){
		
	int *client, server;
	int portNum = 1500;
	bool isExit = false;

	pthread_t thread;
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	struct sockaddr_in server_addr;
	socklen_t size;

	//init socket

	server = socket(AF_INET, SOCK_STREAM, 0);

	if(server < 0){
		cout << "Error establishing connection."<< endl;
		exit(1);
	}
	
	cout<< "Server Socket connection created..."<< endl;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(portNum);

	//binding socket

	if(bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) <0){
		cout<< "Error binding socket..."<< endl;
		exit(1);
	}

	size = sizeof(server_addr);
	cout << "Looking for client..." << endl;

	//listening socket

	listen(server, 10);

	//accept client
	while(1){
		cout << "Wait for client!!" << endl;
		client = new int;
		*client = accept(server,(struct sockaddr*)&server_addr,&size);
		if(*client<0){
			cout <<"Error Accept..."<< endl;
			exit(1);
		}else{
			cout <<"client want to connect..."<< endl;
			pthread_create(&thread,&attr,ServerWork,(void*)client);
			pthread_detach(thread);
		}
	}

	close(server);


	return NULL;
}


void* ServerWork(void *arg){
	
	int *client = (int *)arg;
	int bufsize = 8192;
	char buffer[bufsize];

	Manager manager;

	strcpy(buffer, "Server connected...\n");
	send(*client,buffer,bufsize,0);

	manager.ChooseMode(*client);

	if(!manager.LogIn(*client,manager)){
		close(*client);
		delete client;
	
		return NULL;
	}

	if(manager.GetisClient() == true){  //client	
		if(manager.GetisMember() == true){ // member
			cout << manager.GetaName() << " is log in!!" << endl;
			while(true){
				string StrBuffer;
		
				recv(*client,buffer,bufsize,0);
				StrBuffer = buffer;
		
				if(buffer[0]=='!'){
					cout <<manager.GetaName()<< " member connect out!!" << endl;
					break;
				}

				if(buffer[0] == 'c'){

					switch(buffer[1]){
						case '1':
							
							server.ClientShowAllStore(*client,manager);

							break;
						case '2':
							
							server.ClientGetStoreTableInfo(*client,manager);

							break;
						case '3':
							
							server.ClientGetStoreMenuInfo(*client,manager);

							break;
						case '4':
							
							server.ClientGetStoreReview(*client,manager);

							break;
						case '5':
							
							server.ClientSetStoreReview(*client,manager);

							break;
						case '6':
							
							server.ClientRecommendStore(*client,manager);

							break;
						case '7':
							
							server.ClientResetPassword(*client,manager);

							break;
						default:
							strcpy(buffer, "Invalid input...\n");
							send(*client,buffer,bufsize,0);
					}
				}else{

					strcpy(buffer, "Invalid input...\n");
					send(*client,buffer,bufsize,0);

				}
			}
		}else{ //No member

			while(true){
				string StrBuffer;
		
				recv(*client,buffer,bufsize,0);
				StrBuffer = buffer;
		
				if(buffer[0]=='!'){
					cout << "no member connect out!!" << endl;
					break;
				}

				if(buffer[0] == 'c'){

					switch(buffer[1]){
						case '1':
							
							server.ClientShowAllStore(*client,manager);
				
							break;
						case '2':
							
							server.ClientGetStoreTableInfo(*client,manager);

							break;
						case '3':
							
							server.ClientGetStoreMenuInfo(*client,manager);

							break;
						// If you want, insert more!!	
						default:
							strcpy(buffer, "Invalid input...\n");
							send(*client,buffer,bufsize,0);
					}
				}else{
					strcpy(buffer, "Invalid input...\n");
					send(*client,buffer,bufsize,0);

				}
			}
		}
	}else{ // store
		cout << manager.GetaName() << " is log in!!" << endl;
		while(true){
			string StrBuffer;
	
			recv(*client,buffer,bufsize,0);
			StrBuffer = buffer;
	
			if(buffer[0]=='!'){
				cout <<manager.GetaName() << " store connect out!!" << endl;
				break;
			}

			if(buffer[0] == 's'){

				switch(buffer[1]){
					case '1':
						
						server.StoreGetStoreTableInfo(*client,manager);

						break;
					case '2':

						server.StoreGetStoreMenuInfo(*client,manager);

						break;
					case '3':

						server.StoreGetStoreReview(*client,manager);

						break;
					case '4':

						server.StoreClearMenuAndReset(*client,manager);

						break;
					case '5':

						server.StoreClearTableAndReset(*client,manager);

						break;
					case '6':

						server.StoreClearReview(*client,manager);

						break;
					case '7':

						server.StoreResetPassword(*client,manager);

						break;
					case '8':

						server.StoreFullTable(*client,manager);

						break;
					case '9':

						server.StoreEmptyTable(*client,manager);

						break;
					default:
						strcpy(buffer, "Invalid input...\n");//not reached!!
						send(*client,buffer,bufsize,0);
				}
				}else{

				strcpy(buffer, "Invalid input...\n");
				send(*client,buffer,bufsize,0);

			}
		}
	}

	close(*client);
	delete client;
	
	return NULL;
}


void Server::StartServer(void){
	int bufsize = 8192;
	char buffer[bufsize];
	bool ret_val;

	pthread_attr_init(&aAttr);
	pthread_create(&aThread, &aAttr, ServerWaitClient, NULL);
  pthread_mutex_init(&aClientAccount_lock,NULL);
	pthread_mutex_init(&aClientInfo_lock,NULL);
	pthread_mutex_init(&aStoreAccount_lock,NULL);
	pthread_mutex_init(&aStoreGenre_lock,NULL);
	pthread_mutex_init(&aStoreInfo_lock,NULL);


	//All DB open
	strcpy(buffer,"aClientAccount");
	ret_val = aClientAccount.OpenDB(buffer);
	if(ret_val == true){
		cout << "aClientAccount DB open!!"<< endl;
	}else{
		cout << "aclientAccount DB open fail..."<< endl;
		exit(1);
	}

	strcpy(buffer,"aClientInfo");
	ret_val = aClientInfo.OpenDB(buffer);
	if(ret_val == true){
		cout << "aClientInfo DB open!!"<< endl;
	}else{
		cout << "aclientInfo DB open fail..."<<endl;
		exit(1);
	}

	strcpy(buffer,"aStoreAccount");
	ret_val = aStoreAccount.OpenDB(buffer);
	if(ret_val ==true){
		cout << "aStoreAccount DB open!!"<< endl;
	}else{
		cout << "aStoreAccout DB open fail!!"<< endl;
		exit(1);
	}

	strcpy(buffer,"aStoreGenre");
	ret_val = aStoreGenre.OpenDB(buffer);
	if(ret_val == true){
		cout << "aStoreGenre DB open!!" << endl;
	}else{
		cout << "aStoreGenre DB open fail ..." << endl;
		exit(1);	
	}

	strcpy(buffer,"aStoreInfo");
	ret_val = aStoreInfo.OpenDB(buffer);
	if(ret_val == true){
		cout << "aStoreInfo DB open!!" << endl;
	}else{
		cout << "aStoreInfo DB open fail..." << endl;
	}

	//Table confirm
	cout <<"ClientAccountInfo Table Confirming..."<< endl;
	strcpy(buffer,"ClientAccountInfo * identity text * password text #");
	aClientAccount.CreateTable(2,buffer);

	cout <<"ClientInfo Table confirming..."<< endl;
	strcpy(buffer,"ClientInfo * identity text * email text #");
	aClientInfo.CreateTable(2,buffer);

	cout <<"ClientHistory Table confirming..."<<endl;
	strcpy(buffer,"ClientHistory * identity text * Korean text * American text * Chinese text * Japanese text * Other text #");
	aClientInfo.CreateTable(6,buffer);

	cout <<"StoreAccountInfo Table confirming..."<<endl;
	strcpy(buffer,"StoreAccountInfo * name text * password text #");
	aStoreAccount.CreateTable(2,buffer);

	cout <<"StoreGenre Table confirming..."<<endl;
	strcpy(buffer,"StoreGenre * name text * genre text #");
	aStoreGenre.CreateTable(2,buffer);

	cout <<"StoreInfo Table confirming..."<<endl;
	strcpy(buffer,"StoreInfo * name text * tableinfo text * menu text #");
	aStoreInfo.CreateTable(3,buffer);

	cout <<"StoreReview Table confirming..."<<endl;
	strcpy(buffer,"StoreReview * name text * point text * cnt text * review text #");
	aStoreInfo.CreateTable(4,buffer);

}


void Server::EndServer(void){

	pthread_cancel(aThread);
	aClientAccount.CloseDB();
	aClientInfo.CloseDB();
	aStoreAccount.CloseDB();
	aStoreGenre.CloseDB();
	//each store DB close
}

/************** Manager Class Method ******************/


void Manager::ChooseMode(int client){
	int bufsize = 8192;
	char buffer[bufsize];
	string str;

	while(true){
		recv(client,buffer,bufsize,0);
		str = buffer;
		if(str == "client"){
			
			strcpy(buffer,"You are client...");
			send(client,buffer,bufsize,0);
			isClient = true;
			break;

		}else if(str == "store"){
			
			strcpy(buffer,"You are store...");
			send(client,buffer,bufsize,0);
			isClient = false;
			break;

		}else{
			
			strcpy(buffer,"Invalid mode choose again");
			send(client,buffer,bufsize,0);

		}
	}
	
	if(isClient == true){
		cout << "Client mode in..." << endl;
	}else{
		cout << "Store mode in..." << endl;
	}
}


bool Manager::LogIn(int client, Manager &manager){
	int bufsize = 8192;
	char buffer[bufsize];
	string str;

	if(isClient == true){
		while(true){
			recv(client,buffer,bufsize,0);
			str = buffer;
			if(str == "member"){
				//logIn service
			
				if(server.LogInClient(client,manager) == true){
					strcpy(buffer,"You are member");
					send(client,buffer,bufsize,0);
				
					isMember = true;
					cout << " member log In..."<< endl;
					break;
				}

			}else if(str == "nomember"){
				strcpy(buffer,"You are not member");
				send(client,buffer,bufsize,0);
				isMember = false;
				strcpy(buffer,"nomember");
				manager.SetaName(buffer);
				cout << " no member log in" << endl;
				break;
			}else if(str == "register"){

				if(server.RegisterClient(client)==true){
					cout << "Client Register success!!"<<endl;
					strcpy(buffer,"Register success!!");
					send(client,buffer,bufsize,0);

					//break;
				}

			}else if(str == "finish"){
				strcpy(buffer,"Bye");
				send(client,buffer,bufsize,0);
				cout << "Client Go out!!"<<endl;

				return false;
				//break;

			}else{
				strcpy(buffer,"Invalid option!!");
				send(client,buffer,bufsize,0);
			}
		}
	}else{
		while(true){
			recv(client,buffer,bufsize,0);
			str = buffer;
			if(str == "login"){
				//log in work

				if(server.LogInStore(client,manager) ==true){
					strcpy(buffer,"You are store member");
					send(client,buffer,bufsize,0);
					cout <<" store member log in" << endl;
					break;
				}
			}else if(str == "register"){
				if(server.RegisterStore(client)==true){
					cout << "Store register success"<<endl;
					strcpy(buffer,"Register success");
					send(client,buffer,bufsize,0);

				}
			}else if(str == "finish"){
				strcpy(buffer,"Bye");
				send(client,buffer,bufsize,0);
				cout << "Store Go out!!"<<endl;

				return false;
			}else{
				strcpy(buffer,"Invalid option!!");
				send(client,buffer,bufsize,0);
			}
		}
	}

	return true;
}

void Manager::SetaName(char* buffer){
	string str;
	str = buffer;
	aName = str;
}

/***************Class Manager Method End****************/



bool Server::RegisterClient(int client){
	
	pthread_mutex_lock(&aClientAccount_lock);

	int bufsize = 8192;
	char buffer[bufsize];
	int ret_val;

	strcpy(buffer,"Input ID and password!!");
	send(client,buffer,bufsize,0);
	recv(client,buffer,bufsize,0);

	string temp;
	string id;
	string password;

	temp = buffer;
	stringstream ss(temp);

	string temStr;
	string str;

	ss >> id;
	ss >> password;
	ss >> password;


	temStr = "ClientAccountInfo * identity * ";
	temStr += id;
	strcpy(buffer,&temStr[0]);

	ret_val = aClientAccount.FindData(buffer);

	if(ret_val != -1){
		pthread_mutex_unlock(&aClientAccount_lock);
		strcpy(buffer,"ID already exist!!");
		send(client,buffer,bufsize,0);
		return false;
	}


	temStr = "ClientAccountInfo * ";
	
	
	temStr += id + " * " + password + " #" ;
	
	strcpy(buffer,&temStr[0]);
	aClientAccount.UpsertData(buffer);

	str = "ClientAccountInfo";
	strcpy(buffer,&str[0]);
	aClientAccount.PrintTable(buffer);
	cout << endl;

//Additional Info
	strcpy(buffer, "Input your Email address!!");
	send(client,buffer,bufsize,0);
	recv(client,buffer,bufsize,0);

	temStr = "ClientInfo * " + id + " * ";
	str = buffer;
	temStr += str + " #";
	
	pthread_mutex_lock(&aClientInfo_lock);

	strcpy(buffer,&temStr[0]);
	aClientInfo.UpsertData(buffer);
	strcpy(buffer,"ClientInfo");
	aClientInfo.PrintTable(buffer);
	cout << endl;

	temStr = "ClientHistory * " + id + " * 0 * 0 * 0 * 0 * 0 #";
	strcpy(buffer,&temStr[0]);
	aClientInfo.UpsertData(buffer);
	strcpy(buffer,"ClientHistory");
	aClientInfo.PrintTable(buffer);
	cout << endl;

	pthread_mutex_unlock(&aClientInfo_lock);
	pthread_mutex_unlock(&aClientAccount_lock);
	
	return true;
}


bool Server::RegisterStore(int client){
	
	pthread_mutex_lock(&aStoreAccount_lock);

	int bufsize = 8192;
	char buffer[bufsize];
	int ret_val;

	strcpy(buffer,"Input name and Password!!");
	send(client,buffer,bufsize,0);
	recv(client,buffer,bufsize,0);
	
	string name;
	string password;

	string temp;
	string str;

	temp = buffer;
	stringstream ss(temp);
	
	ss >> name;
	ss >> password;
	ss >> password;

	ss.clear();
	temp = "StoreAccountInfo * name * ";
	temp += name + " #";

	strcpy(buffer,&temp[0]);
	ret_val = aStoreAccount.FindData(buffer);

	if(ret_val != -1){
		pthread_mutex_unlock(&aStoreAccount_lock);
		strcpy(buffer,"Store is already exist!!");
		send(client,buffer,bufsize,0);
		return false;
	}


	temp= "StoreAccountInfo * ";

	temp+= name + " * " + password + " #";

	strcpy(buffer,&temp[0]);
	
	aStoreAccount.UpsertData(buffer);


	str = "StoreAccountInfo";
	strcpy(buffer,&str[0]);
	aStoreAccount.PrintTable(buffer);
	cout << endl;

//Additional Info
	strcpy(buffer,"success");
	send(client,buffer,bufsize,0);

	while(true){
		recv(client,buffer,bufsize,0);
		cout <<"recv data = " <<buffer << endl;
		string para;
		int ret_int;

		str = buffer;
		ss.clear();
		ss.str(str);

		ss >> temp; // genre
		para = "StoreGenre * ";
		para += name + " * " + temp + " #";

		ss >> temp; // *
		ss >> temp; // tablenum
		ss >> temp; // real num

		ret_int = atoi(&temp[0]);
		if(ret_int <=0){
			cout << "Invalid input" << endl;
			strcpy(buffer,"Invalid Input!!");
			send(client,buffer,bufsize,0);
			continue;
		}
		string para2;	
		para2 = "StoreInfo * ";
		para2 += name + " * ";
		para2 += temp + " / ";

		ss>> temp; // /
		
		while(ret_int>0){
			ss >> temp;
			para2 += temp + " ";
			ss >> temp;
			para2 += temp + " ";

			ret_int--;
		}

		para2 += "* ";
		ss >> temp; //*
		ss >> temp; //menunum

		ss >> temp; //menu num
		ret_int = atoi(&temp[0]);
		if(ret_int <=0){
			cout <<"Invalid input"<< endl;
			strcpy(buffer,"Invalid Input");
			send(client,buffer,bufsize,0);
			continue;
		}

		para2 += temp  + " /";

		ss >> temp; // /
		while(ret_int >0){
			ss>> temp;
			para2 += " " + temp;
			ss>> temp;
			para2 += " " + temp;

			ret_int--;
		}

		para2 += " #";

		cout << para<< endl;
		cout << para2 << endl;

		pthread_mutex_lock(&aStoreGenre_lock);

		strcpy(buffer,&para[0]);
		aStoreGenre.UpsertData(buffer);
		para = "StoreGenre";
		strcpy(buffer,&para[0]);
		aStoreGenre.PrintTable(buffer);
		cout << endl;
		
		pthread_mutex_unlock(&aStoreGenre_lock);

		pthread_mutex_lock(&aStoreInfo_lock);

		strcpy(buffer,&para2[0]);
		aStoreInfo.UpsertData(buffer);
		para2 = "StoreInfo";
		strcpy(buffer,&para2[0]);
		aStoreInfo.PrintTable(buffer);
		cout  << endl;

		para = "StoreReview * " + name + " * 0 * 0 * / #";
		strcpy(buffer,&para[0]);
		aStoreInfo.UpsertData(buffer);
		para = "StoreReview";
		strcpy(buffer,&para[0]);
		aStoreInfo.PrintTable(buffer);
		cout << endl;

		pthread_mutex_unlock(&aStoreInfo_lock);


		break;
	}
	pthread_mutex_unlock(&aStoreAccount_lock);
	
	return true;
}


bool Server::LogInClient(int client, Manager &manager){

	pthread_mutex_lock(&aClientAccount_lock);

	int bufsize = 8192;
	char buffer[bufsize];
	string str;
	int ret_value;

	strcpy(buffer,"Input your ID and password");
	send(client,buffer,bufsize,0);
	recv(client,buffer,bufsize,0);
				
	string temp;
	string temp2;
	temp = buffer;
	stringstream ss(temp);
				
	ss >> str;
	
	strcpy(buffer,&str[0]);
	manager.SetaName(buffer);

	temp2 = "ClientAccountInfo * identity * ";
	temp2 += str + " #";

	strcpy(buffer,&temp2[0]);

	ret_value = aClientAccount.FindData(buffer);

	if(ret_value == -1){
		strcpy(buffer,"ID does not exist!!");
		send(client,buffer,bufsize,0);
		pthread_mutex_unlock(&aClientAccount_lock);
		return false;
	}

	//str is password
	ss>>str;
	ss>>str;
	
	temp2 = "ClientAccountInfo";
	strcpy(buffer,&temp2[0]);

	temp2 = aClientAccount.GetData(ret_value,buffer);
	
	ss.clear();
	ss.str(temp2);
	
	ss >> temp;
	ss >> temp;
	ss >> temp;

	if(str != temp){
		strcpy(buffer,"Password is worng!!");
		send(client,buffer,bufsize,0);
		pthread_mutex_unlock(&aClientAccount_lock);
		return false;
	}

	pthread_mutex_unlock(&aClientAccount_lock);

	return true;

}


bool Server::LogInStore(int client, Manager &manager){
	
	pthread_mutex_lock(&aStoreAccount_lock);

	int bufsize = 8192;
	char buffer[bufsize];
	string str;
	int ret_value;

	strcpy(buffer,"Input your store name and password");
	send(client,buffer,bufsize,0);
	recv(client,buffer,bufsize,0);
				
	string temp;
	string temp2;
	temp = buffer;
	stringstream ss(temp);
				
	ss >> str;

	strcpy(buffer,&str[0]);
	manager.SetaName(buffer);

	temp2 = "StoreAccountInfo * name * ";
	temp2 += str + " #";

	strcpy(buffer,&temp2[0]);

	ret_value = aStoreAccount.FindData(buffer);

	if(ret_value == -1){
		strcpy(buffer,"Store does not exist!!");
		send(client,buffer,bufsize,0);
		pthread_mutex_unlock(&aStoreAccount_lock);
		return false;
	}

	//str is password
	ss>>str;
	ss>>str;
	
	temp2 = "StoreAccountInfo";
	strcpy(buffer,&temp2[0]);

	temp2 = aStoreAccount.GetData(ret_value,buffer);
	
	ss.clear();
	ss.str(temp2);
	
	ss >> temp;
	ss >> temp;
	ss >> temp;

	if(str != temp){
		strcpy(buffer,"Password is worng!!");
		send(client,buffer,bufsize,0);
		pthread_mutex_unlock(&aStoreAccount_lock);
		return false;
	}

	pthread_mutex_unlock(&aStoreAccount_lock);
	return true;
}


void Server::StoreGetStoreTableInfo(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " get store table info..." << endl;
	strcpy(buffer,"Store get table information ...");
	send(client,buffer,bufsize,0);

	int ret_int;
	string str;

	str = "StoreInfo * name * ";
	str += manager.GetaName();
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"Invaild");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreInfo");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}

}


void Server::StoreGetStoreMenuInfo(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " get store menu info... " << endl;
	strcpy(buffer,"Store get menu information...");
	send(client,buffer,bufsize,0);

	int ret_int;
	string str;

	str = "StoreInfo * name * ";
	str += manager.GetaName();
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"Invaild");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreInfo");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}

}


void Server::StoreGetStoreReview(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " get store review... " << endl;
	strcpy(buffer,"Store get review...");
	send(client,buffer,bufsize,0);

	int ret_int;
	string str;

	str = "StoreReview * name * ";
	str += manager.GetaName();
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"Invaild");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreReview");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}

}


void Server::StoreClearMenuAndReset(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " clear and reset menu... " << endl;
	strcpy(buffer,"Store clear and reset menu...");
	send(client,buffer,bufsize,0);

	
	int ret_int;
	string str;

	str = "StoreInfo * name * ";
	str += manager.GetaName();
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"Invaild");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreInfo");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}

	string temp;
	recv(client,buffer,bufsize,0);
	temp = buffer;
	

	pthread_mutex_lock(&aStoreInfo_lock);
	
	strcpy(buffer,"StoreInfo");
	aStoreInfo.DeleteData(ret_int,buffer);
	str = "StoreInfo * "+temp;
	strcpy(buffer,&str[0]);
	aStoreInfo.UpsertData(buffer);

	pthread_mutex_unlock(&aStoreInfo_lock);

}


void Server::StoreClearTableAndReset(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " clear and reset table... " << endl;
	strcpy(buffer,"Store clear and reset table...");
	send(client,buffer,bufsize,0);
	
	int ret_int;
	string str;

	str = "StoreInfo * name * ";
	str += manager.GetaName();
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"Invaild");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreInfo");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}

	string temp;
	recv(client,buffer,bufsize,0);
	temp = buffer;
	

	pthread_mutex_lock(&aStoreInfo_lock);
	
	strcpy(buffer,"StoreInfo");
	aStoreInfo.DeleteData(ret_int,buffer);
	str = "StoreInfo * "+temp;
	strcpy(buffer,&str[0]);
	aStoreInfo.UpsertData(buffer);

	pthread_mutex_unlock(&aStoreInfo_lock);

}


void Server::StoreClearReview(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " clear review... " << endl;
	strcpy(buffer,"Store clear review...");
	send(client,buffer,bufsize,0);
	
	int ret_int;
	string str;

	str = "StoreReview * name * " + manager.GetaName() + " #";
	strcpy(buffer,&str[0]);

	pthread_mutex_lock(&aStoreInfo_lock);
	
	ret_int = aStoreInfo.FindData(buffer);

	strcpy(buffer,"StoreReview");
	aStoreInfo.DeleteData(ret_int,buffer);

	str = "StoreReview * " + manager.GetaName() + " * 0 * 0 * / #";
	strcpy(buffer,&str[0]);
	aStoreInfo.UpsertData(buffer);
	strcpy(buffer,"StoreReview");
	aStoreInfo.PrintTable(buffer);

	pthread_mutex_unlock(&aStoreInfo_lock);

}


void Server::StoreResetPassword(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " reset password... " << endl;
	strcpy(buffer,"Store reset password...");
	send(client,buffer,bufsize,0);
	
	int ret_int;
	string str;

	string newPassword;

	recv(client,buffer,bufsize,0);
	newPassword = buffer;

	str = "StoreAccountInfo * name * " + manager.GetaName() + " #";
	strcpy(buffer,&str[0]);

	pthread_mutex_lock(&aStoreAccount_lock);

	ret_int = aStoreAccount.FindData(buffer);

	strcpy(buffer,"StoreAccountInfo");
	aStoreAccount.DeleteData(ret_int,buffer);

	str = "StoreAccountInfo * " + manager.GetaName() + " * ";
	str += newPassword + " #";
	strcpy(buffer,&str[0]);

	aStoreAccount.UpsertData(buffer);
	strcpy(buffer,"StoreAccountInfo");
	aStoreAccount.PrintTable(buffer);

	pthread_mutex_unlock(&aStoreAccount_lock);


	strcpy(buffer,&newPassword[0]);
	send(client,buffer,bufsize,0);

}


void Server::StoreFullTable(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " full table... " << endl;
	strcpy(buffer,"Store full table...");
	send(client,buffer,bufsize,0);

	recv(client,buffer,bufsize,0);
	
	string str;
	string temp;
	string update;
	int ret_int;
	int where;

	where = atoi(buffer);

	temp = "StoreInfo * name * " + manager.GetaName() + " #";
	strcpy(buffer,&temp[0]);

	pthread_mutex_lock(&aStoreInfo_lock);
	
	ret_int = aStoreInfo.FindData(buffer);
	strcpy(buffer,"StoreInfo");
	str = aStoreInfo.GetData(ret_int,buffer);
	stringstream ss(str);

	pthread_mutex_unlock(&aStoreInfo_lock);

	update = "StoreInfo * ";
	ss >> temp; //name
	update += temp + " ";
	ss >> temp; //*
	update += temp + " ";
	ss >> temp; //table num

	int times;
	int index =1;

	times = atoi(&temp[0]);
	update += temp + " ";

	if(where <= 0 || times < where){
		strcpy(buffer,"That table does not exist!!");
		send(client,buffer,bufsize,0);
		return;
	}

	ss >> temp; // /
	update += temp + " ";

	while(times>0){
		if(index== where){
			ss >> temp;
			update += temp+ " ";
			ss >> temp;
			update += "1 ";
		}else{
			ss >>temp;
			update += temp+ " ";
			ss >>temp;
			update += temp+ " ";
		}

		times--;
		index++;
	}

	while(ss >> temp){
		update += temp + " ";
	}
	pthread_mutex_lock(&aStoreInfo_lock);
	
	strcpy(buffer,"StoreInfo");
	aStoreInfo.DeleteData(ret_int,buffer);
	strcpy(buffer,&update[0]);
	aStoreInfo.UpsertData(buffer);

	pthread_mutex_unlock(&aStoreInfo_lock);


	strcpy(buffer,"table full work!!");
	send(client,buffer,bufsize,0);

}


void Server::StoreEmptyTable(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " empty table... " << endl;
	strcpy(buffer,"Store empty table...");
	send(client,buffer,bufsize,0);

	recv(client,buffer,bufsize,0);
	
	string str;
	string temp;
	string update;
	int ret_int;
	int where;

	where = atoi(buffer);

	temp = "StoreInfo * name * " + manager.GetaName() + " #";
	strcpy(buffer,&temp[0]);

	pthread_mutex_lock(&aStoreInfo_lock);
	
	ret_int = aStoreInfo.FindData(buffer);
	strcpy(buffer,"StoreInfo");
	str = aStoreInfo.GetData(ret_int,buffer);
	stringstream ss(str);

	pthread_mutex_unlock(&aStoreInfo_lock);

	update = "StoreInfo * ";
	ss >> temp; //name
	update += temp + " ";
	ss >> temp; //*
	update += temp + " ";
	ss >> temp; //table num

	int times;
	int index =1;

	times = atoi(&temp[0]);
	update += temp + " ";

	if(where <= 0 || times < where){
		strcpy(buffer,"That table does not exist!!");
		send(client,buffer,bufsize,0);
		return;
	}

	ss >> temp; // /
	update += temp + " ";

	while(times>0){
		if(index== where){
			ss >> temp;
			update += temp+ " ";
			ss >> temp;
			update += "0 ";
		}else{
			ss >>temp;
			update += temp+ " ";
			ss >>temp;
			update += temp+ " ";
		}

		times--;
		index++;
	}

	while(ss >> temp){
		update += temp + " ";
	}
	pthread_mutex_lock(&aStoreInfo_lock);
	
	strcpy(buffer,"StoreInfo");
	aStoreInfo.DeleteData(ret_int,buffer);
	strcpy(buffer,&update[0]);
	aStoreInfo.UpsertData(buffer);

	pthread_mutex_unlock(&aStoreInfo_lock);


	strcpy(buffer,"table empty work!!");
	send(client,buffer,bufsize,0);
	
}

void Server::ClientShowAllStore(int client,Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " Show All Store... " << endl;
	strcpy(buffer,"Show All Store!!");
	send(client,buffer,bufsize,0);

	string str;

	strcpy(buffer, "StoreGenre");

	pthread_mutex_lock(&aStoreGenre_lock);
	aStoreGenre.GetTable(buffer,buffer);
	pthread_mutex_unlock(&aStoreGenre_lock);

	send(client,buffer,bufsize,0);

}


void Server::ClientGetStoreTableInfo(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " Get store's table info... " << endl;
	strcpy(buffer,"Get Store Table Info");
	send(client,buffer,bufsize,0);

	recv(client,buffer,bufsize,0);
	string name = buffer;
	string str;
	int ret_int;


	str = "StoreInfo * name * ";
	str += name;
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"fail");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreInfo");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}


}


void Server::ClientGetStoreMenuInfo(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " Get store's menu info " << endl;
	strcpy(buffer,"Get Store Menu Info");
	send(client,buffer,bufsize,0);

	recv(client,buffer,bufsize,0);
	string name = buffer;
	string str;
	int ret_int;


	str = "StoreInfo * name * ";
	str += name;
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"fail");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreInfo");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}



}


void Server::ClientGetStoreReview(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " Get store's review " << endl;
	strcpy(buffer,"Get Store Review");
	send(client,buffer,bufsize,0);

	string name;
	recv(client,buffer,bufsize,0);
	name = buffer;
	
	int ret_int;
	string str;

	str = "StoreReview * name * ";
	str += name;
	str += " #";
	
	strcpy(buffer,&str[0]);
	
	pthread_mutex_lock(&aStoreInfo_lock);
	ret_int = aStoreInfo.FindData(buffer);
	pthread_mutex_unlock(&aStoreInfo_lock);

	if(ret_int == -1){
		strcpy(buffer,"Invaild");
		send(client,buffer,bufsize,0);
		cout << "no store table..."<< endl;
	}else{
		strcpy(buffer,"StoreReview");

		pthread_mutex_lock(&aStoreInfo_lock);
		str = aStoreInfo.GetData(ret_int,buffer);
		pthread_mutex_unlock(&aStoreInfo_lock);

		cout <<"*log= "<<str<< endl;
		strcpy(buffer, &str[0]);
		send(client,buffer,bufsize,0);
	}



}


void Server::ClientSetStoreReview(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " Set store's review " << endl;
	strcpy(buffer,"Set Store Review");
	send(client,buffer,bufsize,0);

	recv(client,buffer,bufsize,0);

	int ret_int1, ret_int2;	
	string store;
	string str;
	string str2;
	string temp;
	string head;

	str2 = buffer;
	stringstream ss(str2);

	ss >> store;

	str = "StoreReview * name * " + store + " #";
	strcpy(buffer,&str[0]);

	pthread_mutex_lock(&aStoreInfo_lock);

	ret_int1 = aStoreInfo.FindData(buffer);

	pthread_mutex_unlock(&aStoreInfo_lock);

	str = "StoreGenre * name * " + store + " #";
	strcpy(buffer,&str[0]);

	pthread_mutex_lock(&aStoreGenre_lock);

	ret_int2 = aStoreGenre.FindData(buffer);

	pthread_mutex_unlock(&aStoreGenre_lock);

	if(ret_int1==-1 || ret_int2==-1){
		strcpy(buffer,"Store does not exist!!");
		send(client,buffer,bufsize,0);
	}else{
		int point;
		int total_cnt;
		int temp_point;

		string sGenre;
	
		strcpy(buffer, "StoreGenre");

		pthread_mutex_lock(&aStoreGenre_lock);

		sGenre = aStoreGenre.GetData(ret_int2, buffer);

		pthread_mutex_unlock(&aStoreGenre_lock);

		head = "StoreReview * "+ store + " * ";
		ss >> store; // *
		ss >> store; // point

		temp_point = stoi(store);

		ss >> store; // *

		temp = "/ " + manager.GetaName() + " ";

		while(ss >> store){
			if(store == "#"){
				break;
			}else{
				temp += store+ " ";
			}
		}

		strcpy(buffer,"StoreReview");
		pthread_mutex_lock(&aStoreInfo_lock);

		str = aStoreInfo.GetData(ret_int1,buffer);
		aStoreInfo.DeleteData(ret_int1,buffer);

		ss.clear();
		ss.str(str);
		ss >> store; // name
		ss >> store; // *
		ss >> store; // point

		point = stoi(store);

		ss >> store; // *
		ss >> store; // cnt

		total_cnt = stoi(store);

		point *= total_cnt;
		total_cnt++;
		point += temp_point;
		point /= total_cnt;

		head += to_string(point) + " * " + to_string(total_cnt) + " * ";

		ss >> store; // *

		while(ss >>store){
			temp += store+ " ";
		}

		head += temp;

		strcpy(buffer,&head[0]);
		aStoreInfo.UpsertData(buffer);

		pthread_mutex_unlock(&aStoreInfo_lock);
	
		strcpy(buffer,"upload review...");
		send(client,buffer,bufsize,0);

		str = "ClientHistory * identity * " + manager.GetaName() + " #";
		strcpy(buffer,&str[0]);

		pthread_mutex_lock(&aClientInfo_lock);

		ret_int1 = aClientInfo.FindData(buffer);

		if(ret_int1 != -1){
			string cHistory;
			string genre;
			int Kor;
			int Ame;
			int Chi;
			int Jap;
			int Oth;

			strcpy(buffer,"ClientHistory");

			cHistory = aClientInfo.GetData(ret_int1, buffer);

			aClientInfo.DeleteData(ret_int1, buffer);

			ss.clear();
			ss.str(cHistory);
			ss >> temp;
			ss >> temp;
			ss >> temp;
			Kor = stoi(temp);
			ss >> temp;
			ss >> temp;
			Ame = stoi(temp);
			ss >> temp;
			ss >> temp;
			Chi = stoi(temp);
			ss >> temp;
			ss >> temp;
			Jap = stoi(temp);
			ss >> temp;
			ss >> temp;
			Oth = stoi(temp);

			ss.clear();
			ss.str(sGenre);
			ss >> genre;
			ss >> genre;
			ss >> genre;

			if(genre == "Korean"){
				Kor++;
			}else if(genre == "American"){
				Ame++;
			}else if(genre == "Chinese"){
				Chi++;
			}else if(genre == "Japanese"){
				Jap++;
			}else if(genre == "other"){
				Oth++;
			}else{
//cout << "Genre : " << genre << endl;
			}

			str = "ClientHistory * " + manager.GetaName() + " * " + to_string(Kor) + " * " + to_string(Ame) + " * " 
						+ to_string(Chi) + " * " + to_string(Jap) + " * " + to_string(Oth) + " #";

			strcpy(buffer,&str[0]);

			aClientInfo.UpsertData(buffer);
		}else{

		}

		pthread_mutex_unlock(&aClientInfo_lock);
	}
}

void Server::ClientRecommendStore(int client, Manager manager){

	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " want Recommend Store " << endl;
	strcpy(buffer,"Recommend Store");
	send(client,buffer,bufsize,0);

	int ret_int;
	string str;

	str = "ClientHistory * identity * " + manager.GetaName() + " #";
	strcpy(buffer,&str[0]);

	pthread_mutex_lock(&aClientInfo_lock);

	ret_int = aClientInfo.FindData(buffer);

	if(ret_int != -1){
		string cHistory;
		string temp;
		int Kor;
		int Ame;
		int Chi;
		int Jap;
		int Oth;
		int priority;
		deque<int> id_set;
		deque<string> name_set;
		deque<int> point_set;

		strcpy(buffer,"ClientHistory");

		cHistory = aClientInfo.GetData(ret_int, buffer);

		stringstream ss(cHistory);

		ss >> temp;
		ss >> temp;
		ss >> temp;
		Kor = stoi(temp);
		ss >> temp;
		ss >> temp;
		Ame = stoi(temp);
		ss >> temp;
		ss >> temp;
		Chi = stoi(temp);
		ss >> temp;
		ss >> temp;
		Jap = stoi(temp);
		ss >> temp;
		ss >> temp;
		Oth = stoi(temp);

		if((Kor < Oth) && (Ame < Oth) && (Chi < Oth) && (Jap < Oth)){
			priority = 4;
		} else if((Kor < Jap) && (Ame < Jap) && (Chi < Jap)){
			priority = 3;
		} else if((Kor < Chi) && (Ame < Chi)){
			priority = 2;
		} else if(Kor < Ame) {
			priority = 1;
		} else{
			priority = 0;
		}

		int genre_cnt = 0;

		if(priority == 0) {
			str = "StoreGenre * genre * Korean #";
		} else if(priority == 1){
			str = "StoreGenre * genre * American #";
		} else if(priority == 2){
			str = "StoreGenre * genre * Chinese #";
		} else if(priority == 3){
			str = "StoreGenre * genre * Japanese #";
		} else if(priority == 4){
			str = "StoreGenre * genre * other #";
		} else{

		}

		strcpy(buffer,&str[0]);

		pthread_mutex_lock(&aStoreGenre_lock);

		id_set = aStoreGenre.GetWantData(buffer);

		strcpy(buffer,"StoreGenre");

		for(int i = 0; i < id_set.size(); i++){
			str = aStoreGenre.GetData(id_set.at(i), buffer);

			ss.clear();
			ss.str(str);
			ss >> temp;

			name_set.push_back(temp);
		}

		pthread_mutex_unlock(&aStoreGenre_lock);

		int total_size = name_set.size();

		if(total_size <= 10){
			str = " * ";

			for(int i = 0; i < total_size; i++){
				str += name_set.at(i) + " * ";
			}

			strcpy(buffer,&str[0]);
			send(client,buffer,bufsize,0);
		}else{
			pthread_mutex_lock(&aStoreInfo_lock);

			for(int i = 0; i < total_size; i++){
				int point;

				str = "StoreReview * name * " + name_set.at(i) + " #";
				strcpy(buffer,&str[0]);

				ret_int = aStoreInfo.FindData(buffer);

				strcpy(buffer, "StoreReview");
				str = aStoreInfo.GetData(ret_int, buffer);

				ss.clear();
				ss.str(str);
				ss >> temp;
				ss >> temp;
				ss >> temp;

				point = stoi(temp);

				point_set.push_back(point);

			}

			pthread_mutex_unlock(&aStoreInfo_lock);

			int loop_cnt = 0;
			int max_point;
			int index;
			deque<string>::iterator name_index;
			deque<int>::iterator point_index;
			deque<string>::iterator name_fix;
			deque<int>::iterator point_fix;

			str = " * ";
			while(loop_cnt != 10){
				max_point = point_set.front();
				total_size = name_set.size();
				name_index = name_set.begin();
				point_index = point_set.begin();
				name_fix = name_index;
				point_fix = point_index;
				index = 0;

				for(int i = 0; i < total_size; i++){
					if(max_point < point_set.at(i)){
						max_point = point_set.at(i);
						name_fix = name_index;
						point_fix = point_index;
						index = i;
					}

					name_index++;
					point_index++;
				}

				str += name_set.at(index) + " * ";
				name_set.erase(name_fix);
				point_set.erase(point_fix);

				loop_cnt++;
			}

			strcpy(buffer,&str[0]);
			send(client,buffer,bufsize,0);
		}

	}else{

	}

	pthread_mutex_unlock(&aClientInfo_lock);


}

void Server::ClientResetPassword(int client, Manager manager){
	
	int bufsize = 8192;
	char buffer[bufsize];

	cout << manager.GetaName() << " Reset password " << endl;
	strcpy(buffer,"Reset Password");
	send(client,buffer,bufsize,0);
	
	int ret_int;
	string str;

	string newPassword;

	recv(client,buffer,bufsize,0);
	newPassword = buffer;

	str = "ClientAccountInfo * identity * " + manager.GetaName() + " #";
	strcpy(buffer,&str[0]);

	pthread_mutex_lock(&aClientAccount_lock);

	ret_int = aClientAccount.FindData(buffer);

	strcpy(buffer,"ClientAccountInfo");
	aClientAccount.DeleteData(ret_int,buffer);

	str = "ClientAccountInfo * " + manager.GetaName() + " * ";
	str += newPassword + " #";
	strcpy(buffer,&str[0]);

	aClientAccount.UpsertData(buffer);
	strcpy(buffer,"ClientAccountInfo");
	aClientAccount.PrintTable(buffer);

	pthread_mutex_unlock(&aClientAccount_lock);


	strcpy(buffer,&newPassword[0]);
	send(client,buffer,bufsize,0);


}
