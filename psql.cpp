#include "psql.h"

connection* create_connection(){
  try{
    //establish a connection to the database
    //param: db name, username, user password
    connection* C = new connection("dbname=bank user=Sharon");
    if(C->is_open()){
      std::cout << "open db successfully: " << C->dbname() << std::endl;
    }
    else{
      std::cout << "cannot open db" << std::endl;
      exit(1);
    }
    return C;
  }
  catch(const std::exception &e){
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}

void create_tables(connection* C){
  try{
    //create tables: ACCOUNT, TRANSFERS
    std::string account("ACCOUNT");
    std::string create_account = "CREATE TABLE " + account + "(" + "ACCOUNT_NUM BIGINT NOT NULL PRIMARY KEY, BALANCE DECIMAL(17, 2) NOT NULL" + ")";
    std::string drop_account = "DROP TABLE IF EXISTS " + account;
    work W_account(*C);
    W_account.exec(drop_account);
    W_account.exec(create_account);
    W_account.commit();
    std::cout << "ACCOUNT created" << std::endl;
    
    std::string transfer("TRANSFER");
    std::string create_transfer = "CREATE TABLE " + transfer + "(" + "REF TEXT, ACCOUNT_TO BIGINT NOT NULL, ACCOUNT_FROM BIGINT NOT NULL, AMOUNT DECIMAL(17,2), TAGS TEXT" + ")";
    std::string drop_transfer = "DROP TABLE IF EXISTS " + transfer;
    work W_transfer(*C);
    W_transfer.exec(drop_transfer);
    W_transfer.exec(create_transfer);
    W_transfer.commit();
    std::cout << "TRANSFER created" << std::endl;
  }
  catch(const std::exception &e){
    std::cerr << e.what() << std::endl;
    exit(1);
  }
  return;
}

int insert_acc(connection *C, std::string& str_acc, std::string& str_bal){
  try{
    std::string table ("ACCOUNT");
    std::string pre ("INSERT INTO ");
    std::string mid ("VALUES (");
    std::string post = ");";
    std::string comma = ",";
    std::string attributes = "(ACCOUNT_NUM,BALANCE)";
    std::string init = pre + table + attributes + mid;
    std::string insert_q("");
    
    insert_q = init + str_acc + comma + str_bal + post;
    //std::cout << "insert:" << insert_q << std::endl;
    work W_create(*C);
    W_create.exec(insert_q);
    W_create.commit();
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return 0;
  }
  return 1;
}

int insert_trs(connection* C, std::string& str_ref, std::string& str_to, std::string& str_from, std::string& str_bal, std::string& str_tag){
  try{
    std::string tb_trs("TRANSFER");
    std::string pre_insert ("INSERT INTO ");
    std::string mid_insert ("VALUES (");
    std::string post = ");";
    std::string comma = ",";
    std::string attributes = "(REF,ACCOUNT_TO,ACCOUNT_FROM,AMOUNT,TAGS)";
    std::string init_trs = pre_insert + tb_trs + attributes + mid_insert;
    
    std::string insert_q = init_trs + "'" + str_ref + "'"+ comma + str_to + comma + str_from + comma + str_bal + comma + "'" + str_tag + "'" + post;

    work W_insert(*C);
    W_insert.exec(insert_q);
    W_insert.commit();
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return 0;
  }
  return 1;
}

int update_acc(connection* C, std::string& update_acc, std::string& amount){
  try{
    std::string pre_update("UPDATE ACCOUNT AS a SET BALANCE=");
    std::string mid_update(" WHERE a.ACCOUNT_NUM=");
    std::string post = ";";
    std::string update_q("");
    update_q =pre_update + amount + mid_update + update_acc + post;
    //    std::cout << "update query: " << update_to << std::endl;
    work W_update(*C);
    W_update.exec(update_q);
    W_update.commit();
    
  } catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return 0;
  }
  return 1;
}

double retrieve_bal(connection* C, std::string& str_acc){
  double res;
  try{
    std::string init_retrv = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_NUM=";
    std::string post(";");
    
    std::string retrv_q = init_retrv  + str_acc + post;
    
    nontransaction N(*C);
    result R(N.exec(retrv_q));
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      res = c[0].as<double>();
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return std::numeric_limits<double>::min();
  }
  return res;
}


int retrieve_acc(connection* C, std::string& str_acc){
  try{
    std::string init = "SELECT EXISTS (SELECT ACCOUNT_NUM FROM ACCOUNT WHERE ACCOUNT_NUM=";
    std::string post = ");";
    std::string retrieve_q = init + str_acc + post;

    nontransaction N(*C);
    result R(N.exec(retrieve_q));
    bool res;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
       res = c[0].as<bool>();
       if(res == true){
	 return 1;
       }
       else if(res == false){
	 return 0;
       }
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return 0;
  }
  return 1;
}
