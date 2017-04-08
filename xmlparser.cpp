#include "xmlparser.h"

std::string readXML(const char* filename){
  std::stringstream ss;
  std::string line;
  std::ifstream file(filename);
  if(file.is_open()){
    while(std::getline(file, line)){
      line.push_back('\n');
      ss << line;
    }
  }
  else{
    fprintf(stderr,"Server: Can not open file\n");
    exit(1);
  }
  return ss.str();
}

void insert_declaration(std::unordered_map<std::string, std::string>& dec, TiXmlDocument& doc){
  TiXmlDeclaration * decl = new TiXmlDeclaration(dec["version"].c_str(), dec["encoding"].c_str(), dec["standalone"].c_str());
  doc.LinkEndChild( decl );
}

void insert_to_doc(TiXmlDocument& doc, std::string value, std::string message){
  TiXmlElement * element = new TiXmlElement(value.c_str());
  if(message != ""){
    TiXmlText * text = new TiXmlText(message.c_str());
    element->LinkEndChild( text );
  }
  doc.LinkEndChild( element );
}

void insert_element_to_parent(TiXmlNode & parent, std::string value, std::string message, std::string ref){
  TiXmlElement * element = new TiXmlElement(value.c_str() );
  element->SetAttribute("ref", ref.c_str());
  TiXmlText * text = new TiXmlText(message.c_str());
  element->LinkEndChild( text );
  parent.LinkEndChild( element );
}

void add_to_map(TiXmlElement* child, std::unordered_map<std::string, std::string>* map_p, std::string key){
  if(child && child->GetText()){
    (*map_p)[key] = std::string(child->GetText());
  }
  else{
    (*map_p)[key] = "N/A";
  }
}

void get_dec(std::unordered_map<std::string, std::string>& dec_m, TiXmlDocument& doc){
  TiXmlDeclaration* dec = doc.FirstChild()->ToDeclaration();
  if(dec){
    dec_m["version"] = std::string(dec->Version());
    dec_m["encoding"] = std::string(dec->Encoding());
    dec_m["standalone"] = std::string(dec->Standalone());
  }
  else{
    printf("dec null\n");
  }
  return;
}

std::string get_reset(TiXmlElement* rootEle){
  std::string reset;
  if(rootEle->Attribute("reset")){
    if(std::string(rootEle->Attribute("reset")) == "true"){
      reset = "true";
    }
    else if(std::string(rootEle->Attribute("reset")) == "false"){
      reset = "false";
    }
    else{
      reset = "N/A";
    }
  }
  else{
    reset = "N/A";
  }
  return reset;
}

void write_xml_response(TiXmlNode& parent, bool result, std::string res, std::unordered_map<std::string, std::string>& map){
  if(result == true){
    insert_element_to_parent(parent, std::string("success"), res, map["ref"]);
    }
  else {
    insert_element_to_parent(parent, std::string("error"), res, map["ref"]);
  }
}

std::string match_db_name(std::string str){
  std::string res;
  if(str == "to"){
    res = "account_to";
  }
  else if(str == "from"){
    res = "account_from";
  }
  else if(str == "amount"){
    res = "amount";
  }
  else{
    res = "error";
    std::cerr<< "invalid query: not tp/from/amount \n";
  }
  return res;
}
void parse_query(TiXmlElement* parent, int* error, std::string& err_str, pqxx::connection* C){
  TiXmlElement* firstEle = parent->FirstChildElement();
  TiXmlElement* tranEle;

  if(parent){
    for(tranEle = firstEle; 
	 tranEle != 0;
	 tranEle = tranEle->NextSiblingElement()){
      
      if(std::string(tranEle->Value()) == "and"){
	parse_query(tranEle, error, err_str, C);
      }
      else if(std::string(tranEle->Value()) == "or"){
	parse_query(tranEle,error, err_str, C);
      }
      else if(std::string(tranEle->Value()) == "not"){
	parse_query(tranEle,error, err_str,C);
      }
      else if(std::string(tranEle->Value()) == "greater" || std::string(tranEle->Value()) == "less" || std::string(tranEle->Value()) == "equals"){
	
	std::string opr = std::string(tranEle->Value());
	TiXmlAttribute* pAttrib = tranEle->FirstAttribute();
	std::string target = match_db_name(std::string(pAttrib->Name()));
	if(target == "error"){
	  *error = 1;
	  err_str = "invalid query: not tp/from/amount ";
	  return;
	}
	std::string num = std::string(pAttrib->Value());
	std::vector<std::unordered_map< std::string, std::string> > res;
	retrieve_tranf_logic(C, target, opr, num, res);
	
	/*
	std::cout << "**********************\n";
	printf("size of res = %lu\n", res.size());
	for(int i = 0; i < res.size(); i++){
	  std::cout << (res[i])["ref"] << " " <<  (res[i])["account_to"] << " " << (res[i])["account_from"] << " "  <<  (res[i])["amount"] << " " <<  (res[i])["tags"] << " \n";  
	}
	std::cout << "***********************\n";
	*/
      }
      else if(std::string(tranEle->Value()) == "tag"){
	const char* info = tranEle->Attribute("info");
	std::string tag = std::string(info);
	std::vector<std::unordered_map< std::string, std::string> > res;
	retrieve_tranf_tag(C, tag, res);
      }
      else{
	std::cerr << "invalid  query: invalid tag\n";
	*error = 1;
	err_str = "invalid query: not tp/from/amount ";
	return;
      }
    }
  } 
  return;
}


char*  parse(char* buff, int ref_count, pqxx::connection* C, int* len){
  // request doc
  TiXmlDocument doc;
  TiXmlElement* tranEle = 0;
  TiXmlElement* rootEle = 0;
  int count = 0;

  // resposne doc
  TiXmlDocument rep_doc;

  // put the str in the parser
  doc.Parse(buff);
  rootEle = doc.RootElement();
 
  // declaration
  std::unordered_map<std::string, std::string> dec;
  get_dec(dec, doc);
  insert_declaration(dec, rep_doc);
  insert_to_doc(rep_doc, std::string("results"), std::string(""));
  
  // reset
  std::string reset = get_reset(rootEle);
  if(reset == "true"){
    std::string tb = std::string("account");
    clear_table(C, tb);
    tb = std::string("transfer");
    clear_table(C, tb);
  }
  // content
  for(tranEle = rootEle->FirstChildElement(); 
      tranEle;
      tranEle = tranEle->NextSiblingElement()){
  
    count++;

    // initialize map_p, res, result
    std::unordered_map<std::string, std::string> map;
    bool result = false;
    std::string res("");
    
    // verb
    std::string verb = std::string(tranEle->Value());
    map["verb"] = verb;    

    // ref
    if(tranEle->Attribute("ref")){
      map["ref"] = std::string(tranEle->Attribute("ref"));
    }
    else{
      std::string reff = "given" + std::to_string(ref_count);
      ref_count++;
      map["ref"] = reff;
    }

    // create
    if(verb == "create"){
      TiXmlElement* child = tranEle->FirstChildElement("account");
      add_to_map(child, &map, "account");
      child = tranEle->FirstChildElement("balance");
      add_to_map(child, &map, "balance");
      map["finished"] = "0";
      result = create(C, map, res);
    }

    // transfer
    else if(verb == "transfer"){
      TiXmlElement* child = tranEle->FirstChildElement("to");
      add_to_map(child, &map, "to");
      child = tranEle->FirstChildElement("from");
      add_to_map(child, &map, "from");
      child = tranEle->FirstChildElement("amount");
      add_to_map(child, &map, "amount");
      std::string tags;
      for(TiXmlElement* tag = tranEle->FirstChildElement("tag");
	  tag;
	  tag = tag->NextSiblingElement("tag")){
	if(tag && tag->GetText()){
	  tags += tag->GetText();
	  tags += ",";
	}
      }
      tags.pop_back();
      map["tags"] = tags;
      result = transfer(C, map, res);
    }

    // balance
    else if(verb == "balance"){
      TiXmlElement* child = tranEle->FirstChildElement("account");
      add_to_map(child, &map, "account");
      result = balance(C, map, res);
    }

    // query
    else if(verb == "query"){
      int error;
      std::string err_str;
      parse_query(tranEle, &error, err_str, C);
      if(error == 1){
	// TODO: write error response
	
      }
      
      // use it's own write to xml func
      continue;
    }

    // invliad verb
    else{
      std::cout<<"undefine transaction"<< std::endl;
      result = false;
      res = "invalid transaction";
    }
    
    // write respose
    TiXmlNode * results = rep_doc.FirstChild("results");
    if(results){
      write_xml_response(*results, result, res, map);
    }
  }


  // write xml to memory buffer
  TiXmlPrinter printer;
  printer.SetIndent( "\t" );
  rep_doc.Accept( &printer );
  *len = printer.Size();
  char* xml_rep = (char*) malloc((*len) * sizeof(char));
  memcpy(xml_rep, printer.CStr(), (*len));
  printf("xml parser: size = %d\n", *len);
  return xml_rep;
}



