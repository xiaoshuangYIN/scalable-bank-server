#include "server_base.h"

void set_socket(addrinfo* hints_p){
  memset(hints_p, 0, sizeof(*hints_p));
  hints_p->ai_family = AF_UNSPEC;
  hints_p->ai_socktype = SOCK_STREAM;
  hints_p->ai_flags = AI_PASSIVE; // use my IP
  return;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void get_addr_info(struct addrinfo* hints_p, struct addrinfo** servinfo, int* rv){
  if ((*rv = getaddrinfo(NULL, PORT, hints_p, servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(*rv));
    exit(1);
  }
}

void bind_sock(struct addrinfo** servinfo, int* sockfd_p, int* yes_p){
  // loop through all the results and bind to the first we can
  struct addrinfo* p;
  int fd;
  for(p = *servinfo; p != NULL; p = p->ai_next) {
    if ((fd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes_p,
		   sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    
    if (bind(fd, p->ai_addr, p->ai_addrlen) == -1){
      close(fd);
      perror("server: bind");
      continue;
    }
    break;
  }
  
  freeaddrinfo(*servinfo); // all done with this structure

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  *sockfd_p = fd;
  return;
}

int accept_sock(int sockfd, struct sockaddr_storage their_addr){
  socklen_t sin_size = sizeof (their_addr);
   int new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
  if (new_fd == -1) {
    perror("accept");
  }
  return new_fd; 
}

void sigchld_handler(int s){
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;
  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}

void sigaction(struct sigaction* sa_p){
  sa_p->sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa_p->sa_mask);
  sa_p->sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, sa_p, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
  return;
}

void listen_sock(int* sockfd_p){
  if (listen(*sockfd_p, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  return;
}


void close_sock(int* sockfd_p){
  close(*sockfd_p);
  return;
}

uint64_t recv_size_byte(int* sockfd_p){
  uint64_t* len_p = (uint64_t*)malloc(8); 
  int numbytes;                                                
  if ((numbytes = read(*sockfd_p, len_p, 8)) != 8) {
    perror("recv XML request from client failed");
    exit(1);
  }
  uint64_t len = ntohll(*len_p);
  printf("server: received '%llu'\n", len);
  free(len_p);
  return len;
}


void recv_content(int* sockfd_p, char* buff_p, int len){
  int numbytes;
  if ((numbytes = recv(*sockfd_p, buff_p, len+1, 0)) == -1) {
    perror("recv XML request from client failed");
    exit(1);
  }
  buff_p[len] = '\0';
  //printf("%s", buff_p);
  return;
}



void* requestThread(void* para){
  threadPara* para_t = (threadPara*)para;
  int sockfd = para_t->new_fd;

  /* receive XML */
  uint64_t len = recv_size_byte(&sockfd);
  char buff[len+8];
  recv_content(&sockfd, buff, len);
  
  /* parse */
  std::vector<std::unordered_map<std::string, std::string>* >* trans 
    = new std::vector<std::unordered_map<std::string, std::string >* >();
  
  //declaration dec_t;

  std::unordered_map<std::string, std::string> dec;
  std::string reset = parse(buff, trans, para_t->transfers, para_t->ref_count, dec);

  printf("end: version: %s\n", dec["version"].c_str());
  printf("encoding: %s\n", dec["encoding"].c_str());
  printf("std: %s\n", dec["standalone"].c_str());

  
  /* test */
  /*
  for(int i = 0; i < (*trans).size(); i++){
    std::cout<< (*(*trans)[i])["verb"] << std::endl;
  }
  */
  return 0;
}
