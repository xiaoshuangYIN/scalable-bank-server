#include "navigator.h"


//navigate to each operation for one request
void navigator( std::unordered_map<std::string, std::string>& table, connection* C, TiXmlNode& results){

  std::cout<<"navigator...\n";
  std::unordered_map<std::string, std::string >::const_iterator it = table.find("verb");
  std::string res("");
  bool result;
  // account operation
  if (it != table.end()){
    if (it->second == "create"){
      result = create(C, table, res);
    }
    else if(it->second == "transfer"){
      result = transfer(C, table, res);
      std::cout << "transfer result: " << result  << std::endl;
    }
    else if(it->second == "query"){
      std::cout << "query result: " << result  << std::endl;
    }
    else if(it->second == "balance"){
      result = balance(C, table, res);
    }
  }
  else{
    std::cerr <<"no verb\n";
  }

  // write respose
  if(result == true){
    std::cout<<"******* ref =  " << table["ref"] << " : " << res << std::endl;
    insert_element_to_parent(results, std::string("success"), res, table["ref"]);
  }
  else {
    std::cout<<"******* ref =  " << table["ref"] << " : " << res << std::endl;
    insert_element_to_parent(results, std::string("error"), res, table["ref"]);
  }
  return;
}

void bank(std::vector<std::unordered_map<std::string, std::string>* >& v_op, connection* C, TiXmlDocument& doc, std::string str){
  TiXmlNode * results = doc.FirstChild(str.c_str() );
  int count = v_op.size();
  for(size_t i = 0; i < v_op.size();i++){
    navigator(*(v_op[i]), C, *results);
    count--;
    printf("count = %d\n", count);
  }
  return;
}
