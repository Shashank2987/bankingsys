#include "filehandler.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

vector<Account> load_accounts() {
    vector<Account> accounts;

    ifstream file("accounts.csv");

    // auto-create file if missing
    if (!file.is_open()) {
        ofstream newfile("accounts.csv");
        newfile << "account_no,name,password,balance\n";
        newfile.close();
        return accounts;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string acc, name, pass, bal;

        getline(ss, acc, ',');
        getline(ss, name, ',');
        getline(ss, pass, ',');
        getline(ss, bal, ',');

        if (acc.empty()) continue;

        accounts.push_back(Account(
            stoi(acc),
            name,
            pass,
            stod(bal)
        ));
    }

    file.close();
    return accounts;
}

void save_accounts(vector<Account> &accounts) {
    ofstream file("accounts.csv");

    file << "account_no,name,password,balance\n";

    for (auto &acc : accounts) {
        file << acc.get_account_no() << ","
             << acc.get_name() << ","
             << acc.get_password() << ","
             << acc.get_balance() << "\n";
    }

    file.close();
}