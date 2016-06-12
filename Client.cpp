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


class Client
{
	public:

		void StartClient(void);
		void WorkClient(void);
		void EndClient(void);

	private:

		int aClient;
		int aPortNum;
		int aBufSize;
		char *aBuffer;
		char *aIp;
		bool isMember;

		bool aLogIn(void);
	
		void aPrintMember(void);
		void aPrintNoMember(void);
		
		void aShowAllStore(void);
		void aGetStoreTableInfo(void);
		void aGetStoreMenuInfo(void);
		void aGetStoreReview(void);
		void aSetStoreReview(void);
		void aRecommendStore(void);
		void aResetPassword(void);
};


static Client client;


int main(int argc, char* argv[]){
	
	client.StartClient();
	client.WorkClient();
	client.EndClient();

	return 0;
}


void Client::StartClient(void){
	
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


void Client::WorkClient(void){

	string str;

	if(!aLogIn()){
		//aBuffer[0] = '!';
		//send(aClient,aBuffer,aBufSize,0);
		return;
	}

	if(isMember== true){ //member service
		while(true){
			aPrintMember();
			getline(cin,str);
			
			if(str == "1"){

				aShowAllStore();

			}else if(str == "2"){
			
				aGetStoreTableInfo();

			}else if(str == "3"){

				aGetStoreMenuInfo();

			}else if(str == "4"){

				aGetStoreReview();

			}else if(str == "5"){

				aSetStoreReview();

			}else if(str == "6"){

				aRecommendStore();

			}else if(str == "7"){

				aResetPassword();

			}else if(str == "8"){

				aBuffer[0] = '!';
				send(aClient,aBuffer,aBufSize,0);
				break;

			}else{
				cout << "Invalid Input!!"<< endl;
			}
		}
	}else{ //no member service
		while(true){
			aPrintNoMember();
			
			getline(cin,str);

			if(str == "1"){

				aShowAllStore();

			}else if(str == "2"){
			
				aGetStoreTableInfo();

			}else if(str == "3"){

				aGetStoreMenuInfo();

			}else if(str == "4"){

				aBuffer[0] = '!';
				send(aClient,aBuffer,aBufSize,0);
				break;

			}else{
				cout << "Invalid Input!!"<< endl;
			}	
		}
	}
}


void Client::EndClient(void){

	
	cout << "connection terminated..." << endl;
	cout << "Goodbye" << endl;

	delete[] aBuffer;
	delete[] aIp;

	close(aClient);

}


bool Client::aLogIn(void){

	string str;

	cout << "LogIn start!!"<< endl;

	while(true){
		strcpy(aBuffer, "client");
		send(aClient,aBuffer,aBufSize,0);
		str = aBuffer;
		
		recv(aClient,aBuffer,aBufSize,0);
		str = aBuffer;

		if(str == "You are client..."){
			cout << "Client mode In" << endl;	
			break;
		}
	}

	while(true){
		cout << "1. Member Log In"<< endl;
		cout << "2. No member Log In"<< endl;
		cout << "3. Register"<< endl;
		cout << "4. Finish" << endl;

		getline(cin,str);

		if(str == "1"){
			strcpy(aBuffer,"member");
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			
			// Log In 
			string temp;
			string temp2;
			cout << aBuffer << endl;

			cout <<"your ID = ";
			getline(cin,temp);
			temp2 = temp;
			cout <<"your Password = ";
			getline(cin,temp);
			temp2 += " * " + temp + " #";

			strcpy(aBuffer,&temp2[0]);
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			str = aBuffer;

			if(str == "You are member"){
				cout << "member Log In" << endl;
				isMember = true;
				break;
			}else{
				cout << str<< endl;
			}
		}else if(str == "2"){
			strcpy(aBuffer,"nomember");
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			str = aBuffer;
			
			if(str == "You are not member"){
				cout << "No member Log In"<< endl;
				isMember = false;
				break;
			}
		}else if(str =="3"){
			strcpy(aBuffer,"register");
			send(aClient,aBuffer,aBufSize,0);
			recv(aClient,aBuffer,aBufSize,0);
			cout << aBuffer <<endl;

			string tem;
			cout << "ID =";
			getline(cin,tem);
			str = tem;
			cout << "Password =";
			getline(cin,tem);
			str += " * " + tem + " #";
			strcpy(aBuffer,&str[0]);
			
			send(aClient,aBuffer,aBufSize,0);
			
			recv(aClient,aBuffer,aBufSize,0);
			cout << aBuffer << endl;
			cout << "Email = ";
			getline(cin,str);
			
			strcpy(aBuffer,&str[0]);
			send(aClient,aBuffer,aBufSize,0);
			
			recv(aClient,aBuffer,aBufSize,0);
			cout << aBuffer << endl;
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


void Client::aPrintMember(void){
	cout<<"-------------------------------------"<<endl;
	cout<<"1. Show All Store"<<endl;
	cout<<"2. Get Store Table Information"<<endl;
	cout<<"3. Get Store Menu Information"<<endl;
	cout<<"4. Get Store Review"<<endl;
	cout<<"5. Set Store Review"<<endl;
	cout<<"6. Recommend Store"<<endl;
	cout<<"7. Reset Password"<<endl;
	cout<<"8. Log out"<<endl;
}


void Client::aPrintNoMember(void){
	cout<<"-------------------------------------"<<endl;
	cout<<"1. Show All Store"<<endl;
	cout<<"2. Get Store Table Information"<<endl;
	cout<<"3. Get Store Menu Information"<<endl;
	cout<<"4. Log out"<<endl;
}


void Client::aShowAllStore(void){
	string str;

	strcpy(aBuffer,"c1");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	recv(aClient,aBuffer,aBufSize,0);
	
	string str2;
	str2 = aBuffer;
	stringstream ss(str2);

	ss >> str; // id
	ss >> str; // name
	ss >> str; //genre
	ss >> str; //*

	cout << "name\tgenre"<<endl;

	while(1){
		ss >> str;
		if(str== "#" || str == "*#"){
			cout << endl;
			break;
		}else{
			ss >> str;
			cout << str;
			cout <<"\t";
			ss >> str;
			cout << str << endl;
		}
	}

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Client::aGetStoreTableInfo(void){
	string str;
	string name;

	strcpy(aBuffer,"c2");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout << "Store name ? ";
	getline(cin,str);
	name = str;
	strcpy(aBuffer,&str[0]);
	send(aClient,aBuffer,aBufSize,0);

	recv(aClient,aBuffer,aBufSize,0);
	
	if(aBuffer == "fail"){
		cout << aBuffer << endl;
	}else{
		string str2=aBuffer;
		stringstream ss(str2);
		string temp;

		int times;
		int index=1;

		ss >> temp; //name
		if(name!=temp){
			cout << "fail"<< endl;
			cout <<"Press Enter to do anything..."<<endl;
			getline(cin,str);
			return;
		}
		ss >> temp; //*
		ss >> temp; //times

		times = atoi(&temp[0]);

		ss >> temp; // /
		while(times>0){
			cout << "table" << index <<"'s capacity is ";
			ss >> temp;
			cout << temp;
			ss >> temp;
			if(temp=="0"){
				cout << "  empty" << endl;
			}else{
				cout << "  full" << endl;
			}

			times--;
			index ++;
		}
		
	}

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Client::aGetStoreMenuInfo(void){
	string str;
	string name;

	strcpy(aBuffer,"c3");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout << "Store name ? ";
	getline(cin,str);
	name = str;
	strcpy(aBuffer,&str[0]);
	send(aClient,aBuffer,aBufSize,0);

	recv(aClient,aBuffer,aBufSize,0);

	if(aBuffer == "fail"){
		cout << aBuffer << endl;
	}else{
		string str2 = aBuffer;
		stringstream ss(str2);
		string temp;

		int times;
		int index=1;

		ss >> temp; //name
		if(name!=temp){
			cout << "fail"<< endl;
			cout <<"Press Enter to do anything..."<<endl;
			getline(cin,str);
			return;
		}
		ss >> temp; //*
		ss >> temp; //table num;

		times = atoi(&temp[0]);
		
		ss >> temp; // /
		while(times>0){
			ss >> temp;
			ss >> temp;
			times--;
		}
		ss >> temp; //*

		ss >> temp; //menu num

		times = atoi(&temp[0]);

		ss >> temp; // /
		cout << "menu\tprice" << endl;
		while(times>0){
			ss >> temp;
			cout << temp << "\t";
			ss >> temp;
			cout << temp << endl;

			times--;
			index++;
		}

	}

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}


void Client::aGetStoreReview(void){
	string str;
	string name;

	strcpy(aBuffer,"c4");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout << "Store name ? ";
	getline(cin,str);
	strcpy(aBuffer,&str[0]);
	name = str;
	send(aClient,aBuffer,aBufSize,0);

	recv(aClient,aBuffer,aBufSize,0);
	
	string str2;
	string temp;
	string temp2;
	int index =1;
	int times;
	str2 = aBuffer;
	stringstream ss(str2);

	ss >> temp; //name
	if(name!=temp){
			cout <<"fail"<<endl;
			cout <<"Press Enter to do anything..."<<endl;
			getline(cin,str);
			return;
	}
	ss >> temp; // *
	ss >> temp;	// point

	cout << " - Point : " << temp << " -" << endl;

	ss >> temp; // *
	ss >> temp; // point cnt
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


void Client::aSetStoreReview(void){
	string str;
	string temp;

	strcpy(aBuffer,"c5");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	cout << "Store name ?";
	getline(cin,temp);
	str = temp;
	str += " * ";

	cout << "Store grade ?";
	getline(cin,temp);
	str += temp;
	str += " * ";	

	cout << "Your review (One Line)= ";
	getline(cin,temp);
	str += temp + " #";

	strcpy(aBuffer,&str[0]);
	send(aClient,aBuffer,aBufSize,0);

	recv(aClient,aBuffer,aBufSize,0);

	cout << aBuffer << endl;

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}

void Client::aRecommendStore(void){
	string str;
	string tok;

	strcpy(aBuffer,"c6");
	send(aClient,aBuffer,aBufSize,0);
	recv(aClient,aBuffer,aBufSize,0);
	cout << aBuffer << endl;

	recv(aClient,aBuffer,aBufSize,0);

	cout << endl;

	str = aBuffer;
	stringstream ss(str);

	while(ss >> tok){
		if(tok[0] != '*'){
			cout << " - " << tok << endl;
		}
	}
	//cout << aBuffer << endl;

	cout <<"Press Enter to do anything..."<<endl;
	getline(cin,str);
}

void Client::aResetPassword(void){
	string str;
	string str2;

	strcpy(aBuffer,"c7");
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


