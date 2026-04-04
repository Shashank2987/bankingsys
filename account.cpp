#include "account.h"

Account::Account() {}

Account::Account(int acc, string n, string p, double b) {
    account_no = acc;
    name = n;
    password = p;
    balance = b;
}

int Account::get_account_no() { return account_no; }
string Account::get_name() { return name; }
string Account::get_password() { return password; }
double Account::get_balance() { return balance; }

void Account::deposit(double amt) {
    balance += amt;
}

bool Account::withdraw(double amt) {
    if (amt > balance) return false;
    balance -= amt;
    return true;
}