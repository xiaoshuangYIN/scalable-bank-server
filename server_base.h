#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#include <vector>
#include <string>
#include <unordered_map>
 

//#include "xmlparser.h"
#include "psql.h"
#include "navigator.h"

#define PORT "12345"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold



/** struct **/
struct _threadPara {
  int new_fd;
  std::vector<std::unordered_map<std::string, std::string>* >* transfers;
  int ref_count;
  connection* C; 
};
typedef struct _threadPara threadPara;




void set_socket(addrinfo* hints_po);
void *get_in_addr(struct sockaddr *sa);
void bind_sock(struct addrinfo** servinfo, int* sockfd, int* yes);
void sigchld_handler(int s);
void sigaction(struct sigaction* sa);
void listen_sock(int* sockfd);
void get_addr_info(struct addrinfo* hints, struct addrinfo** servinfo, int* rv);
void close_sock(int* sockfd);
void* requestThread(void* para);
void recv_content(int* sockfd_p, char* buff_p, int len);
uint64_t recv_size_byte(int* sockfd_p);
int accept_sock(int sockfd_p, struct sockaddr_storage their_addr_p);
