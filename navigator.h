#include "bank.h"
#include "psql.h"
using namespace pqxx;
void navigator(std::unordered_map<std::string, std::string>&, connection*);
void bank(std::vector<std::unordered_map<std::string, std::string>* >&, connection*);
