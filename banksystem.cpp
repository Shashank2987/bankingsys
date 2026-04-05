#include "banksystem.h"
#include "filehandler.h"
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

BankSystem::BankSystem() {
    load();
}

void BankSystem::load() {
    accounts = load_accounts();
}

void BankSystem::save() {
    save_accounts(accounts);
}

Account* BankSystem::find_account(int acc_no) {
    for (auto &acc : accounts) {
        if (acc.get_account_no() == acc_no)
            return &acc;
    }
    return nullptr;
}

void BankSystem::create_account() {
    int acc;
    string name, pass;
    double bal;

    cout << "enter account no: ";
    cin >> acc;
    cout << "name: ";
    cin >> name;
    cout << "password: ";
    cin >> pass;
    cout << "initial balance: ";
    cin >> bal;

    accounts.push_back(Account(acc, name, pass, bal));
    save();

    cout << "account created!\n";
}

Account* BankSystem::login() {
    int acc;
    string pass;

    cout << "account no: ";
    cin >> acc;
    cout << "password: ";
    cin >> pass;

    Account* user = find_account(acc);

    if (user && user->get_password() == pass) {
        cout << "login successful!\n";
        return user;
    }

    cout << "invalid credentials\n";
    return nullptr;
}

void BankSystem::check_balance(Account* user) {
    cout << "balance: " << user->get_balance() << endl;
}

void BankSystem::add_money(Account* user) {
    double amt;
    cout << "enter amount: ";
    cin >> amt;

    if (amt <= 0) {
        cout << "invalid amount\n";
        return;
    }

    user->deposit(amt);
    save();

    cout << "money added!\n";
}

void BankSystem::send_money(Account* user) {
    int to_acc;
    double amt;

    cout << "enter receiver account no: ";
    cin >> to_acc;

    cout << "enter amount: ";
    cin >> amt;

    if (amt <= 0) {
        cout << "invalid amount\n";
        return;
    }

    Account* receiver = find_account(to_acc);

    if (!receiver) {
        cout << "receiver not found\n";
        return;
    }

    if (!user->withdraw(amt)) {
        cout << "insufficient balance\n";
        return;
    }

    receiver->deposit(amt);
    save();

    ofstream file("transactions.csv", ios::app);
    time_t now = time(0);

    file << user->get_account_no() << ","
         << receiver->get_account_no() << ","
         << amt << ",transfer,"
         << now << "\n";

    file.close();

    cout << "money sent successfully!\n";
}

void BankSystem::user_menu(Account* user) {
    int choice;

    do {
        cout << "\n1. check balance\n2. add money\n3. send money\n4. exit\n";
        cin >> choice;

        switch (choice) {
            case 1: check_balance(user); break;
            case 2: add_money(user); break;
            case 3: send_money(user); break;
        }

    } while (choice != 4);
}