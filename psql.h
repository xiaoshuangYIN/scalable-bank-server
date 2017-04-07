#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <pqxx/pqxx>
#include <cstring>
#include <iterator>
#include <cstdio>
#include <cerrno>
#include <cstdlib>

using namespace pqxx;

int insert_acc(connection*, std::string&, std::string&);
int insert_trs(connection*, std::string&, std::string&, std::string&, std::string&, std::string&);
int update_acc(connection*, std::string&, std::string&);
double retrieve_bal(connection*, std::string&);
int retrieve_acc(connection*, std::string&);
connection* create_connection();
void create_tables(connection* C);