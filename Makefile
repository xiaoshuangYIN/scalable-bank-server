CC=g++
CFLAGS=-std=c++11
EXTRAFLAGS=-lpqxx -lpq

all: server

server: bank_server.cpp server_base.cpp server_base.h xmlparser.cpp xmlparser.h bank.h bank.cpp psql.cpp psql.h ./tinyxml/tinyxml.cpp ./tinyxml/tinyxmlparser.cpp ./tinyxml/tinyxmlerror.cpp ./tinyxml/tinystr.cpp ./tinyxml/tinyxml.h ./tinyxml/tinystr.h
	$(CC) $(CFLAGS) -o server bank_server.cpp server_base.cpp xmlparser.cpp bank.cpp psql.cpp  ./tinyxml/tinyxml.cpp ./tinyxml/tinyxmlparser.cpp ./tinyxml/tinyxmlerror.cpp ./tinyxml/tinystr.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o server

clobber:
	rm -f *~ *.o

