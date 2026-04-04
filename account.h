#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
using namespace std;

class Account {
private:
    int account_no;
    string name;
    string password;
    double balance;

public:
    Account();
    Account(int acc, string n, string p, double b);

    int get_account_no();
    string get_name();
    string get_password();
    double get_balance();

    void deposit(double amt);
    bool withdraw(double amt);
};

#endif