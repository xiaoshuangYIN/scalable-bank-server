#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <iterator>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <pqxx/pqxx>
#include "psql.h"

using namespace pqxx;

int str2db(const std::string&, std::string&);
int str2ul(const std::string&, std::string&);

bool create(connection*, std::unordered_map<std::string, std::string>&, std::string&);

bool transfer(connection *, std::unordered_map<std::string, std::string> &, std::string &);

bool balance(connection*, std::unordered_map<std::string, std::string> &, std::string &);
