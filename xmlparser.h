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
  char* version;
  char* encoding;
  char* standalone;
};
typedef struct _declaration declaration;

struct _transact{
  bool reset;
  std::vector<std::unordered_map<std::string, std::string> >* map_vec_p;
};

void get_dec(std::unordered_map<std::string, std::string>& dec_m, TiXmlDocument& doc);

std::string parse(char* buff, std::vector <std::unordered_map<std::string, std::string>* >* trans, std::vector<std::unordered_map<std::string, std::string>* >* transfer, int ref_count, std::unordered_map<std::string, std::string>& dec);
void insert_declaration(std::unordered_map<std::string, std::string>& dec, TiXmlDocument& doc);
void insert_to_doc(TiXmlDocument& doc, std::string value, std::string message);
void insert_element_to_parent( TiXmlNode& parent, std::string value, std::string message);
