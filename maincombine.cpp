#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

// ---------------- ACCOUNT CLASS ----------------
class Account {
private:
    int account_no;
    string name;
    string password;
    double balance;

public:
    Account() {}
    Account(int acc, string n, string p, double b) {
        account_no = acc;
        name = n;
        password = p;
        balance = b;
    }

    int get_account_no() { return account_no; }
    string get_name() { return name; }
    string get_password() { return password; }
    double get_balance() { return balance; }

    void deposit(double amt) { balance += amt; }

    bool withdraw(double amt) {
        if (amt > balance) return false;
        balance -= amt;
        return true;
    }
};

// ---------------- FILE HANDLING ----------------
vector<Account> load_accounts() {
    vector<Account> accounts;
    ifstream file("accounts.csv");

    if (!file.is_open()) {
        ofstream newfile("accounts.csv");
        newfile << "account_no,name,password,balance\n";
        newfile.close();
        return accounts;
    }

    string line;
    getline(file, line); // To Skip First Line

    while (getline(file, line)) {
        stringstream ss(line);
        string acc, name, pass, bal;

        getline(ss, acc, ',');
        getline(ss, name, ',');
        getline(ss, pass, ',');
        getline(ss, bal, ',');

        if (acc.empty()) continue;

        accounts.push_back(Account(stoi(acc), name, pass, stod(bal)));
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

// ---------------- SYSTEM FUNCTIONS ----------------
void ensure_transactions_file() {
    ifstream file("transactions.csv");

    if (!file.is_open()) {
        ofstream newfile("transactions.csv");
        newfile << "from_acc,to_acc,amount,type,timestamp\n";
        newfile.close();
    }
}

Account* find_account(vector<Account> &accounts, int acc_no) {
    for (auto &acc : accounts) {
        if (acc.get_account_no() == acc_no)
            return &acc;
    }
    return nullptr;
}

void create_account(vector<Account> &accounts) {
    int acc;
    string name, pass;
    double bal;

    cout << "enter account no: ";
    cin >> acc;

    if (find_account(accounts, acc)) {
        cout << "account already exists\n";
        return;
    }

    cout << "name: ";
    cin >> name;

    cout << "password: ";
    cin >> pass;

    cout << "initial balance: ";
    cin >> bal;

    accounts.push_back(Account(acc, name, pass, bal));
    save_accounts(accounts);

    cout << "account created!\n";
}

Account* login(vector<Account> &accounts) {
    int acc;
    string pass;

    cout << "account no: ";
    cin >> acc;

    cout << "password: ";
    cin >> pass;

    Account* user = find_account(accounts, acc);

    if (user && user->get_password() == pass) {
        cout << "login successful!\n";
        return user;
    }

    cout << "invalid credentials\n";
    return nullptr;
}

void add_money(Account* user, vector<Account> &accounts) {
    double amt;
    cout << "enter amount: ";
    cin >> amt;

    if (amt <= 0) {
        cout << "invalid amount\n";
        return;
    }

    user->deposit(amt);
    save_accounts(accounts);

    cout << "money added!\n";
}

void check_balance(Account* user) {
    cout << "balance: " << user->get_balance() << endl;
}

void send_money(Account* user, vector<Account> &accounts) {
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

    Account* receiver = find_account(accounts, to_acc);

    if (!receiver) {
        cout << "receiver not found\n";
        return;
    }

    if (!user->withdraw(amt)) {
        cout << "insufficient balance\n";
        return;
    }

    receiver->deposit(amt);
    save_accounts(accounts);

    ofstream file("transactions.csv", ios::app);
    time_t now = time(0);

    file << user->get_account_no() << ","
         << receiver->get_account_no() << ","
         << amt << ",transfer,"
         << now << "\n";

    file.close();

    cout << "money sent successfully!\n";
}

void user_menu(Account* user, vector<Account> &accounts) {
    int choice;

    do {
        cout << "\n1. check balance\n2. add money\n3. send money\n4. exit\n";
        cin >> choice;

        switch (choice) {
            case 1: check_balance(user); break;
            case 2: add_money(user, accounts); break;
            case 3: send_money(user, accounts); break;
        }

    } while (choice != 4);
}

// ---------------- MAIN ----------------
int main() {
    cout << "program started\n";

    ensure_transactions_file();
    vector<Account> accounts = load_accounts();

    int choice;

    do {
        cout << "\n1. create account\n2. login\n3. exit\n";
        cin >> choice;

        if (choice == 1) {
            create_account(accounts);
        }
        else if (choice == 2) {
            Account* user = login(accounts);
            if (user)
                user_menu(user, accounts);
        }

    } while (choice != 3);

    return 0;
}