#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>   // Standard C-time
#include <iomanip> // For formatting numbers

using namespace std;

// --- UI UTILS ---
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printLine() {
    cout << "------------------------------------------\n";
}

void printHeader(string title) {
    clearScreen();
    printLine();
    cout << "       " << title << "\n";
    printLine();
}

void pause() {
    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

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
    getline(file, line); 
    while (getline(file, line)) {
        stringstream ss(line);
        string acc, name, pass, bal;
        getline(ss, acc, ',');
        getline(ss, name, ',');
        getline(ss, pass, ',');
        getline(ss, bal, ',');
        if (!acc.empty()) accounts.push_back(Account(stoi(acc), name, pass, stod(bal)));
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
             << fixed << setprecision(2) << acc.get_balance() << "\n";
    }
    file.close();
}

// ---------------- SYSTEM FUNCTIONS ----------------
Account* find_account(vector<Account> &accounts, int acc_no) {
    for (auto &acc : accounts) {
        if (acc.get_account_no() == acc_no) return &acc;
    }
    return nullptr;
}

void create_account(vector<Account> &accounts) {
    printHeader("OPEN NEW ACCOUNT");
    int acc;
    string name, pass;
    double bal;

    cout << "Enter Account No  : "; cin >> acc;
    if (find_account(accounts, acc)) {
        cout << "\n[!] Error: Account already exists.\n";
        pause();
        return;
    }
    cout << "Enter Name        : "; cin.ignore(); getline(cin, name);
    cout << "Enter Password    : "; cin >> pass;
    cout << "Initial Deposit $ : "; cin >> bal;

    accounts.push_back(Account(acc, name, pass, bal));
    save_accounts(accounts);
    cout << "\n[SUCCESS] Account created successfully!\n";
    pause();
}

Account* login(vector<Account> &accounts) {
    printHeader("MEMBER LOGIN");
    int acc;
    string pass;

    cout << "Account No: "; cin >> acc;
    cout << "Password  : "; cin >> pass;

    Account* user = find_account(accounts, acc);
    if (user && user->get_password() == pass) {
        return user;
    }
    cout << "\n[!] Invalid credentials.\n";
    pause();
    return nullptr;
}

void send_money(Account* user, vector<Account> &accounts) {
    printHeader("TRANSFER FUNDS");
    int to_acc;
    double amt;

    cout << "Target Account No : "; cin >> to_acc;
    cout << "Transfer Amount $ : "; cin >> amt;

    if (amt <= 0) {
        cout << "\n[!] Invalid amount.\n";
    } else {
        Account* receiver = find_account(accounts, to_acc);
        if (!receiver) {
            cout << "\n[!] Receiver not found.\n";
        } else if (!user->withdraw(amt)) {
            cout << "\n[!] Insufficient balance.\n";
        } else {
            receiver->deposit(amt);
            save_accounts(accounts);

            // Using ctime for timestamp
            time_t now = time(0);
            char* dt = ctime(&now);

            ofstream file("transactions.csv", ios::app);
            file << user->get_account_no() << "," << receiver->get_account_no() << ","
                 << amt << ",transfer," << dt; // dt includes newline
            file.close();

            cout << "\n[SUCCESS] Transfer complete!\n";
        }
    }
    pause();
}

void user_menu(Account* user, vector<Account> &accounts) {
    int choice;
    do {
        string title = "WELCOME, " + user->get_name();
        printHeader(title);
        cout << "1. Check Balance\n";
        cout << "2. Deposit Money\n";
        cout << "3. Send Money\n";
        cout << "4. Logout\n";
        cout << "\nSelection: ";
        cin >> choice;

        if (choice == 1) {
            cout << "\nYour Balance: $" << fixed << setprecision(2) << user->get_balance() << endl;
            pause();
        } else if (choice == 2) {
            double amt;
            cout << "Amount to Deposit: "; cin >> amt;
            if (amt > 0) {
                user->deposit(amt);
                save_accounts(accounts);
                cout << "\n[SUCCESS] Deposit successful.\n";
            } else cout << "\n[!] Invalid amount.\n";
            pause();
        } else if (choice == 3) {
            send_money(user, accounts);
        }
    } while (choice != 4);
}

int main() {
    vector<Account> accounts = load_accounts();
    int choice;

    do {
        printHeader("CRYPTO BANKING SYSTEM");
        cout << "1. Open Account\n";
        cout << "2. Login\n";
        cout << "3. Exit System\n";
        cout << "\nChoice: ";
        cin >> choice;

        if (choice == 1) create_account(accounts);
        else if (choice == 2) {
            Account* user = login(accounts);
            if (user) user_menu(user, accounts);
        }
    } while (choice != 3);

    return 0;
}