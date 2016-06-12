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