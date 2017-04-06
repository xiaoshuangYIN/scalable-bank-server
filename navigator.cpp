#include "navigator.h"
using namespace pqxx;

//navigate to each operation for one request
void navigator( std::unordered_map<std::string, std::string>& table, connection* C){

  std::cout<<"navigator...\n";
  std::unordered_map<std::string, std::string >::const_iterator it = table.find("verb");
  if (it != table.end()){
    //    std::cout << "verb is: " << it->second << std::endl;

    if (it->second == "create"){
      std::string res("");
      if(!create(C, table, res)){
	//error
	std::cout << "create res: " << res << std::endl;
	return;
      }
      std::cout << "create res: " << res << std::endl;
    }
    else if(it->second == "transfer"){

      std::string res("");
      if(!transfer(C, table, res)){
	//error
	std::cout << "transfer res: " << res << std::endl;
	return;
      }
      std::cout << "transfer res: " << res << std::endl;
    }
    else if(it->second == "query"){
      std::cout << "query " << std::endl;
      // query(it->second); //query some record
      //no default
    }
    else if(it->second == "balance"){
      std::string res("");
      if(!balance(C, table, res)){
	//error
	std::cout << "balance res: " << res << std::endl;
	return;
      }
      std::cout << "balance res: " << res << std::endl;
    }
  }
  else{
    std::cerr <<"no verb\n";
    return;
  }
  return;
}

void bank(std::vector<std::unordered_map<std::string, std::string>* >& v_op, connection* C){

  int count = v_op.size();
  //  printf("count = %d\n", count);
  for(size_t i = 0; i < v_op.size();i++){
    navigator(*(v_op[i]), C);
    //std::cout << "navigate for NO." << i << " operation" << std::endl;
    count--;
    printf("count = %d\n", count);
  }
  return;
}
