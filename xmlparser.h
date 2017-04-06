#include "./tinyxml/tinyxml.h"

#include <unordered_map>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

//using namespace std;

struct _declaration{
  const char* version;
  const char* encoding;
  const char* standalone;
};
typedef struct _declaration declaration;


struct _transact{
  bool reset;
  std::vector<std::unordered_map<std::string, std::string> >* map_vec_p;
};


std::string parse(char* buff, std::vector <std::unordered_map<std::string, std::string>* >* trans, std::vector<std::unordered_map<std::string, std::string>* >* transfer, int ref_count, declaration& dec);
