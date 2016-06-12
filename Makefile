server: Server.cpp Client.cpp Store.cpp mysqlite.o
	g++ -std=c++11 -o server Server.cpp mysqlite.o -lpthread -lsqlite3 
	g++ -std=c++11 -o client Client.cpp -lpthread
	g++ -std=c++11 -o store Store.cpp -lpthread
mysqlite.o: mysqlite.h mysqlite.cpp
	g++ -std=c++11 -c mysqlite.h mysqlite.cpp -lsqlite3
clean:
	rm *.o
	rm *.gch
	rm server
	rm client
	rm store
