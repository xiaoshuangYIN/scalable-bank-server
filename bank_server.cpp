#include "server_base.h" // all the server socket funcs
#include "psql.h"
using namespace std;


/* main */
int main(int argc, char *argv[]){
  
  if(argc < 2){
    printf("usage: ./server xml_response_file_name\n");
  }

  const char* resp_file = argv[1];
  
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  
  struct addrinfo hints;
  struct addrinfo* servinfo;
  
  struct sockaddr_storage their_addr; // connector's address information
  
  socklen_t sin_size;
  
  struct sigaction sa;
  
  int yes=1;
  
  char s[INET6_ADDRSTRLEN];
  
  int rv;

   // create connection
  connection* C = create_connection();

  // create tables
  create_tables(C);
  
  // network
  set_socket(&hints);
  
  get_addr_info(&hints, &servinfo, &rv);
  
  bind_sock(&servinfo, &sockfd, &yes);

  listen_sock(&sockfd);

  sigaction(&sa);
  
  printf("server: waiting for connections...\n");
  
  while(1){
    new_fd = accept_sock(sockfd, their_addr);
    if(new_fd == -1){
      continue;
    }
    
    threadPara para_t;
    para_t.new_fd = new_fd;
    para_t.ref_count = 0;
    para_t.C = C;
    para_t.file = resp_file;
    requestThread((void*)(&para_t));

  }
  close_sock(&sockfd);
  delete C;
  return 0; 
}
