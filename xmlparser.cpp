#include "xmlParser.h"

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
    std::cout<<"******* ref =  " << map["ref"] << " : " << res << std::endl;
    insert_element_to_parent(parent, std::string("success"), res, map["ref"]);
    }
  else {
    std::cout<<"******* ref =  " << map["ref"] << " : " << res << std::endl;
    insert_element_to_parent(parent, std::string("error"), res, map["ref"]);
  }
}

std::string parse(char* buff, int ref_count, pqxx::connection* C){
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
 
  // dec
  std::unordered_map<std::string, std::string> dec;
  get_dec(dec, doc);
  insert_declaration(dec, rep_doc);
  insert_to_doc(rep_doc, std::string("results"), std::string(""));
  
  // reset
  std::string reset = get_reset(rootEle);

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
      std::cout << "query" << std::endl;
    }

    else{
      std::cout<<"undefine transaction"<< std::endl;
    }
    
    // write respose
    TiXmlNode * results = rep_doc.FirstChild("results");
    if(results){
      write_xml_response(*results, result, res, map);
    }
  }
  // save xml to file
  rep_doc.SaveFile("xmlResponse1");
  return reset;
}



