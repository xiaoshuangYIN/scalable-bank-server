#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

//template <typename T>
class Account{
 private:
  unsigned long acc_num;
  double balance;
 public:
 Account(unsigned int init_account): acc_num(init_account), balance(0){}
 Account(unsigned int init_account, double init_balance): acc_num(init_account), balance(init_balance){}
  ~Account(){}
  
  void send(double in){
    this->balance -= in;
    return;
  }
  
  void receive(double out){
    this->balance += out;
    return;
  }
  
  double inquiry(unsigned int q_num){
    if(q_num != this->acc_num){
      std::cerr << "Account number offered does not match" << std::endl;
      return 0;
    }
    //std::cout << "Then current balance of " << q_num << " is: ";
    //std::cout << this->getBalance() << std::endl;
    return this->getBalance();
  }

  unsigned int getAccountNumber() const{
    return this->acc_num;
  }
  
  double getBalance() const{
        return this->balance;
  }
};
