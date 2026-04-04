#include "filehandler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

vector<Account> load_accounts() {
    vector<Account> accounts;
    ifstream file("accounts.csv");

    string line;

    if (!file.is_open()) {
        cout << "file not found\n";
        return accounts;
    }

    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string acc, name, pass, bal;

        getline(ss, acc, ',');
        getline(ss, name, ',');
        getline(ss, pass, ',');
        getline(ss, bal, ',');

        if (acc.empty()) continue;

        int acc_no = stoi(acc);
        double balance = stod(bal);

        Account temp(acc_no, name, pass, balance);
        accounts.push_back(temp);
    }

    file.close();
    return accounts;
}

void save_accounts(vector<Account> &accounts) {
    ofstream file("accounts.csv");

    file << "account_no,name,password,balance\n";

    for (size_t i = 0; i < accounts.size(); i++) {
        file << accounts[i].get_account_no() << ","
             << accounts[i].get_name() << ","
             << accounts[i].get_password() << ","
             << accounts[i].get_balance() << "\n";
    }

    file.close();
}