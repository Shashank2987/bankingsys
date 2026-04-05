#ifndef BANK_SYSTEM_H
#define BANK_SYSTEM_H
#include <vector>
#include "account.h"
using namespace std;
class BankSystem{
    private:
        vector<Account> accounts;
    public:
        BankSystem();
        void load();
        void save();
        void create_account();
        Account*login();
        void user_menu(Account*user);
        Account* find_account(int acc_no);
        void check_balance(Account* user);
        void add_money(Account* user);
        void send_money(Account* user);
};
#endif