#include "server_base.h" // all the server socket funcs
#include "psql.h"
using namespace std;


/* xml reponse file */
string resp_file_string = "xmlReponse1";
const char* resp_file = resp_file_string.c_str();


/* main */
int main(){
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  
  struct addrinfo hints;
  struct addrinfo* servinfo;
  
  struct sockaddr_storage their_addr; // connector's address information
  
  socklen_t sin_size;
  
  struct sigaction sa;
  
  int yes=1;
  
  char s[INET6_ADDRSTRLEN];
  
  int rv;

  // add transfers
  vector<unordered_map<string, string>* >* transfers 
    = new vector<unordered_map<string, string>* >();
  
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
    para_t.transfers = transfers;
    para_t.ref_count = 0;
    para_t.C = C;
    requestThread((void*)(&para_t));

  }
  close_sock(&sockfd);
  free(transfers);
  delete C;
  return 0; 
}
