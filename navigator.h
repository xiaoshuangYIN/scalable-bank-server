#include "bank.h"
#include "psql.h"
#include "xmlparser.h"
using namespace pqxx;
void navigator(std::unordered_map<std::string, std::string>&, connection*, TiXmlNode& results);
void bank(std::vector<std::unordered_map<std::string, std::string>* >&, connection*, TiXmlDocument& doc, std::string str);
