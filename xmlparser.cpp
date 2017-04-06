#include "xmlParser.h"

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
    //printf("from get_dec : ver: %s\n",(*dec_t)->version);
    //printf("encod: %s\n",(*dec_t)->encoding);
    //printf("stan: %s\n",(*dec_t)->standalone);
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
      printf("reset = true\n");
    }
    else if(std::string(rootEle->Attribute("reset")) == "false"){
      reset = "false";
      printf("reset = true\n");
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

std::string parse(char* buff, std::vector<std::unordered_map<std::string, std::string>* >* trans, std::vector<std::unordered_map<std::string, std::string>* >* transfers, int ref_count, std::unordered_map<std::string, std::string>& dec){
  TiXmlDocument doc;
  TiXmlElement* tranEle = 0;
  TiXmlElement* rootEle = 0;
  int count = 0;
  doc.Parse(buff);
  rootEle = doc.RootElement();
 
  // dec
  get_dec(dec, doc);

  //printf("from parse ver: %s\n",(dec_t)->version);
  //printf("encod: %s\n",(dec_t)->encoding);
  //printf("stan: %s\n",(dec_t)->standalone);
 
  // reset
  std::string reset = get_reset(rootEle);

  // content
  for(tranEle = rootEle->FirstChildElement(); 
      tranEle;
      tranEle = tranEle->NextSiblingElement()){
    count++;
    // initialize map_p
    std::unordered_map<std::string, std::string>* map_p = new std::unordered_map<std::string, std::string>();
    // verb
    std::string verb = std::string(tranEle->Value());
    (*map_p)["verb"] = verb;    
    // ref
    if(tranEle->Attribute("ref")){
      (*map_p)["ref"] = std::string(tranEle->Attribute("ref"));
    }
    else{
      std::string reff = "given" + std::to_string(ref_count);
      ref_count++;
      (*map_p)["ref"] = reff;
    }
    // account number
    if(verb == "create"){
      TiXmlElement* child = tranEle->FirstChildElement("account");
      add_to_map(child, map_p, "account");
      child = tranEle->FirstChildElement("balance");
      add_to_map(child, map_p, "balance");
      (*map_p)["finished"] = "0";
    }
    else if(verb == "transfer"){
      TiXmlElement* child = tranEle->FirstChildElement("to");
      add_to_map(child, map_p, "to");
      child = tranEle->FirstChildElement("from");
      add_to_map(child, map_p, "from");
      child = tranEle->FirstChildElement("amount");
      add_to_map(child, map_p, "amount");
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
      (*map_p)["tags"] = tags;
      //std::cout<< (*map_p)["tags"] << std::endl;
      transfers->push_back(map_p);
    }
    else if(verb == "balance"){
      TiXmlElement* child = tranEle->FirstChildElement("account");
      add_to_map(child, map_p, "account");
    }
    else if(verb == "query"){
      std::cout << "query" << std::endl;
    }
    else{
      std::cout<<"undefine transaction"<< std::endl;
    }
    trans->push_back(map_p);
  }
  //printf("vector size: %lu\n", trans->size());
  return reset;
}

