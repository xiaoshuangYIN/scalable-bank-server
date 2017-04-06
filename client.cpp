#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
#define PORT "12345" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// read XML file and put it into a string buffer
string readXML(char* filename){
  stringstream ss;
  string line;
  ifstream file(filename);
  if(file.is_open()){
    while(getline(file, line)){
      line.push_back('\n');
      ss << line;
    }
  }
  else{
    fprintf(stderr,"Client: Can not open file\n");
    exit(1);
  }
  return ss.str();
}

int main(int argc, char *argv[])
{
  int sockfd, numbytes;  
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  /* usage coner case */
  if (argc != 3) {
    fprintf(stderr,"usage: client_hostname XML_file\n");
    exit(1);
  }
  /* socket */
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  /* put xml file into a string */
  char* filename = argv[2];
  string xml_str = readXML(filename);
  uint64_t len = xml_str.size();
  // convert to network byte order
  uint64_t len_big_endien = htonll(len);


  /* get addrinfo */
  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }
  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }
    break;
  }
  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
  printf("client: connecting to %s\n", s);
  freeaddrinfo(servinfo); // all done with this structure
  
  // convert string to char[]
  char xml_array[len+1];
  xml_array[len] = '\0';
  memcpy(xml_array, xml_str.c_str(), len);
  // add 64bits int in front of the char array
  uint64_t* msg = (uint64_t*)malloc(8 + len);
  memcpy(msg, &len_big_endien, sizeof(len_big_endien));
    char* msg_str_p = (char*)(msg + 1);
  memcpy(msg_str_p, xml_array, len);
  int write_bytes = write(sockfd, (const void*)msg, 8+len);
  printf("len send: '%d'\n",write_bytes);
  
  /* receive response from server */
  memset(&buf, 0, sizeof(buf));
  if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    perror("recv XML response from server failed");
    exit(1);
  }
  printf("client: received '%s'\n",buf);
  buf[numbytes] = '\0';
  close(sockfd);
  free(msg);
  return 0;
}
