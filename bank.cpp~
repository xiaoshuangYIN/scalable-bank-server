#include "bank.h"

int str2db( std::string& num, std::string& res){
  //convert a string into floating point, return -1 if contain anything but whitespace and valid numeric representation

  double result;
  std::string str(num);

  //get rid of any trailing whitespace
  if(str.find(" \n\r\t\v\f") != std::string::npos){
    str.erase(str.find_first_of(" \n\r\t\v\f"), str.find_first_not_of(" \n\r\t\v\f"));
  }
  //std::cout << "str after erase is: " << str << std::endl;

  //read it into the target type
  std::istringstream ss(str);
  ss >> result;
  //  std::cout << "result is: " << result << std::endl;
  //check to see if there is nothing left over
  if(!ss.eof()){
    // res = "has balance tag and text, not floating number";
    return 0;
  }
  if(result == 0 || result == 0.0){
    std::size_t is_zero = num.find_first_not_of(" \n\r\t\v\f0.");
    if(is_zero != std::string::npos){
      // res = "has balance tag and text, not floating number";
      return 0;
    }else{
      // std::cout << "result is 0" << std::endl;
      res = std::to_string(result);
      return 1;
    }
  }
  std::ostringstream out;
  std::string::size_type dot = str.find_first_of(".");
  if(dot != std::string::npos){
    //std::cout << "has a dot" << std::endl;
    if(str.find(" \n\r\t\v\f") != std::string::npos){
      str.erase(num.find_first_of(" \n\r\t\v\f"), num.find_first_not_of(" \n\r\t\v\f"));
    }
    int precision = num.size() - 1;
    //(std::to_string(ans));
    out << std::setprecision(precision) << result;
    res = out.str();
    //std::cout << "stream after precision is: " << ss.str() << std::endl;//std::setprecision(1) << std::endl;
  }else{
    out << result;
  }
  res = out.str();
  // std::cout << "valid result return is: " << result << std::endl;
  return 1;
}

int str2ul(std::string& num, std::string& res){
  //convert a string into unsigned integer, return -1 if contain anything but whitespace and valid numeric representation
  double result;
  std::string str(num);

  //get rid of any trailing whitespace
  if(str.find(" \n\r\t\v\f") != std::string::npos){
    str.erase(str.find_first_of(" \n\r\t\v\f"), str.find_first_not_of(" \n\r\t\v\f"));
  }
  //std::cout << "str after erase is: " << str << std::endl;

  //read it into the target type
  std::istringstream ss(str);
  ss >> result;
  //std::cout << "result is: " << result << std::endl;
  //check to see if there is nothing left over
  if(!ss.eof()){
    // res = "has balance tag and text, not floating number";
    return 0;
  }
  if(result == 0 ){
    std::size_t is_zero = num.find_first_not_of(" \n\r\t\v\f0");
    if(is_zero != std::string::npos){
      // res = "has balance tag and text, not floating number";
      return 0;
    }else{
      // std::cout << "result is 0" << std::endl;
      // res = std::to_string(result);
      res = std::to_string(result);
      return 1;
    }
  }
  std::ostringstream out;
  std::string::size_type dot = str.find_last_of(".");
  if(dot != std::string::npos){
    //has a dot in unsigned, invalid
    return 0;
  }else{
    out << result;
  }
  res = out.str();
  // std::cout << "valid result return is: " << result << std::endl;
  return 1;
}

bool create(connection *C, std::unordered_map<std::string, std::string> & t, std::string & res){
  try{
    //for constructing the insert query
    std::string str_acc = t.find("account")->second;
    std::string str_bal = t.find("balance")->second;
    if(str_acc == "N/A"){
      // std::cerr << "No account number offered, unable to create one" << std::endl;
      res = "create: no account tag";
      return false;
    }
    else if(str_acc == "no text"){
      res = "has account tag, no text";
      return false;
    }
    std::string::size_type minus_acc = str_acc.find("-");
    if(minus_acc != std::string::npos){
      //std::cerr << "Invalid: account number must be a 64-bit decimal unsigned integer!" << std::endl;
      res = "create: invalid account number ";
      return false;
    }else{
      if(!str2ul(str_acc, res)){
	res = "has account tag and test, not 64-bit unsigned decimal integer";
	return false;
      }
    }
    std::string::size_type minus_bal = str_bal.find("-");
    if(minus_bal != std::string::npos){
      //std::cerr << "Invalid: initial balance must be non-negative" << std::endl;
      res = "create: invalid balance";
      return false;
    }
    //parse the v, generate an Account object
    else if(str_bal == "N/A"){
      //insert into account table
      std::string def_bal(" 0.00");
      if(!insert_acc(C, str_acc, def_bal)){
	std::cerr << "fail to insert into account table" << std::endl;
      }
      res = "created";

    }
    else if(str_bal == "no text"){
      res = "create: empty balance element";
      return false;
    } else{
      if(!str2db(str_bal, res)){
	res = "has balance tag and text, not floating number";
	return false;
      }else{
	//insert into account table
	if(!insert_acc(C, str_acc, str_bal)){
	  std::cerr << "fail to insert into account table" << std::endl;
	}
	res = "created";
      }
    }
  }catch (const std::exception &e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}

bool transfer(connection *C, std::unordered_map<std::string, std::string> & t, std::string & res){
  try{
    // std::cout << "in transfer" << std::endl;
    std::string str_bal = t.find("amount")->second;
    //    std::cout << "str_bal find in table is: " << str_bal << std::endl;
    std::string str_from = t.find("from")->second;
    std::string str_to = t.find("to")->second;
    std::string str_ref = t.find("ref")->second;
    std::string str_tag = t.find("tags")->second;
    
    if(str_from == "N/A"){
      res = "transfer:no from tag";
      return false;
    }
    if(str_to == "N/A"){
      res = "transfer:no to tag";
      return false;
    }
    if(str_bal == "N/A"){
      res = "transfer:no amount tag";
      return false;
    }
    // std::string::size_type minus_bal = str_bal.find("-");
    // if(minus_bal != std::string::npos){
    //   //std::cerr << "Invalid: initial balance must be non-negative" << std::endl;
    //   res = "transfer: invalid amount text";
    //   return false;
    // }else{
    if(!str2db(str_bal, res)){
      //	std::cout << "str2db after minus" << std::endl;
      res = "transfer: invalid amount text";
      return false;
    }else{
      std::string::size_type sz;
      double amount = std::stod(str_bal, &sz);
      //unsigned long acc_from = std::stoul(str_from, nullptr,10);
      //unsigned long acc_to = std::stoul(str_to, nullptr,10);
      if(!str2ul(str_from, res)){
	// std::cout << "str2ul after from" << std::endl;
	res = "transfer: invalid from text";
	return false;
      }
      else if(!str2ul(str_to, res)){
	//	  std::cout << "str2db after to" << std::endl;
	res = "transfer: invalid to text";
	return false;
      }
      else{
	if(!retrieve_acc(C, str_from)){
	  //   std::cerr << "cannot find from account records, transfer may not complete!" << std::endl;
	  res = "transfer: \"from\" account not found";
	  return false;
	}
	if(!retrieve_acc(C, str_to)){
	  // std::cerr << "cannot find to account records, transfer may not complete!" << std::endl;
	  res = "transfer: \"to\" account not found";
	  return false;
	}
	else{
	  double bal_fm = retrieve_bal(C, str_from);
	  // std::cout << "retrieve bal of from" << std::endl;
	  double bal_to = retrieve_bal(C, str_to);
	  // std::cout << "retrieve bal of to"<< std::endl;
	  if(amount > 0 && bal_fm < amount){
	    res = "transfer: insufficient amount to transfer";
	    return false;
	  }
	  else if(amount < 0 && bal_to < (0-amount)){
	    res = "transfer: insufficient amount to transfer";
	    return false;
	  }
	  else{
	    //update the balance amount of the from account and to account after transfer
	    std::string str_send = std::to_string(bal_fm - amount);
	    if(!update_acc(C, str_from, str_send)){
	      std::cerr << "fail to update the from account" << std::endl;
	    }
	    std::string str_recv = std::to_string(bal_to + amount);
	    if(!update_acc(C, str_to, str_recv)){
	      std::cerr << "fail to update the to account" << std::endl;
	    }
	    
	      //insert this record into transfer table
	    if(!insert_trs(C, str_ref, str_to, str_from, str_bal, str_tag)){
	      std::cerr << "fail to insert into transfer table" << std::endl;
	    }
	    res = "transferred";
	  }
	}
      }
    }
  } catch(const std::exception &e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}

bool balance(connection* C, std::unordered_map<std::string, std::string> & t, std::string & res){
  std::string acc_str = t.find("account")->second;
  if(acc_str == "N/A"){
    //std::cerr << "You cannot inquiry the balance without offering the account number" << std::endl;
    res = "balance: no account tag";
    return false;
  }
  //std::string::size_type sz;
  //unsigned int account = std::stod(acc_str, &sz);
  if(!retrieve_acc(C, acc_str)){
    //no such account exist
    res = "balance: account not found";
    return false;
  }
  double bal = retrieve_bal(C, acc_str);
  res = std::to_string(bal);
  return true;
}
