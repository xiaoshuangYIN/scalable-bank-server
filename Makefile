CC=g++
CFLAGS=-std=c++11
EXTRAFLAGS=-lpqxx -lpq
W=-Wall -Werror

all: bank_server

client: client.cpp
	$(CC) $(W) $(CFLAGS) client.cpp -o client


SRCS := ./tinyxml/tinyxml.cpp ./tinyxml/tinyxmlparser.cpp ./tinyxml/tinyxmlerror.cpp ./tinyxml/tinystr.cpp xmlparser.cpp server_base.cpp bank_server.cpp

SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))

bank_server: ${OBJS} 
	${CC} -o $@ ${OBJS}


%.o : %.cpp                                                                             
	 ${CC} -c ${CFLAGS}  $< -o $@   

account.o : account.h
./tinyxml/tinyxml.o: ./tinyxml/tinyxml.h ./tinyxml/tinystr.h
./tinyxml/tinyxmlparser.o: ./tinyxml/tinyxml.h ./tinyxml/tinystr.h
./tinyxml/xmltest.o: ./tinyxml/tinyxml.h ./tinyxml/tinystr.h
./tinyxml/tinyxmlerror.o: ./tinyxml/tinyxml.h ./tinyxml/tinystr.h




clean:
	rm -f *~ *.o bank_server client

clobber:
	rm -f *~ *.o