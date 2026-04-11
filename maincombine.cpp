#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>

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

        if (!acc.empty())
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
             << fixed << setprecision(2) << acc.get_balance() << "\n";
    }

    file.close();
}
// ---------------- SYSTEM FUNCTIONS ----------------
Account* find_account(vector<Account> &accounts, int acc_no) {
    for (auto &acc : accounts) {
        if (acc.get_account_no() == acc_no)
            return &acc;
    }
    return nullptr;
}
void create_account(vector<Account> &accounts) {
    printHeader("OPEN NEW ACCOUNT");

    int acc;
    string name, pass;
    double bal;

    cout << "Enter Account No  : ";
    cin >> acc;

    if (find_account(accounts, acc)) {
        cout << "\n[!] Account already exists.\n";
        pause();
        return;
    }

    cout << "Enter Name        : ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter Password    : ";
    cin >> pass;

    cout << "Initial Deposit $ : ";
    cin >> bal;

    accounts.push_back(Account(acc, name, pass, bal));
    save_accounts(accounts);

    cout << "\n[SUCCESS] Account created.\n";
    pause();
}

Account* login(vector<Account> &accounts) {
    printHeader("LOGIN");

    int acc;
    string pass;

    cout << "Account No: ";
    cin >> acc;

    cout << "Password  : ";
    cin >> pass;

    Account* user = find_account(accounts, acc);

    if (user && user->get_password() == pass) {
        return user;
    }

    cout << "\n[!] Invalid login.\n";
    pause();
    return nullptr;
}

// ---------------- TRANSACTION HISTORY ----------------
void show_transaction_history(Account* user) {
    printHeader("TRANSACTION HISTORY");

    ifstream file("transactions.csv");

    if (!file.is_open()) {
        cout << "[!] No transaction file found.\n";
        pause();
        return;
    }

    string line;
    bool found = false;

    while (getline(file, line)) {

        if (line.empty()) continue; // 🔥 skip empty lines

        stringstream ss(line);

        string sender, receiver, amount, type, timestamp;

        getline(ss, sender, ',');
        getline(ss, receiver, ',');
        getline(ss, amount, ',');
        getline(ss, type, ',');
        getline(ss, timestamp);

        // 🔥 skip header or invalid rows
        if (sender == "sender" || receiver.empty()) continue;

        try {
            int s = stoi(sender);
            int r = stoi(receiver);
            int acc_no = user->get_account_no();

            if (s == acc_no || r == acc_no) {
                found = true;

                printLine();

                if (s == acc_no) {
                    cout << "[DEBIT]   Sent $" << amount << " to Acc " << receiver << "\n";
                } else {
                    cout << "[CREDIT]  Received $" << amount << " from Acc " << sender << "\n";
                }

                // 🔥 safe timestamp conversion
                time_t t = stol(timestamp);
                cout << "[DATE]    " << ctime(&t);
            }
        }
        catch (...) {
            // 🔥 ignore bad rows instead of crashing
            continue;
        }
    }

    if (!found) {
        cout << "\nNo transactions found.\n";
    }

    file.close();
    pause();
}

//-----------------Fraud Detection-----------------
int calculate_risk(Account* user, int receiver, double amount) {
    int risk = 0;

    double balance = user->get_balance();

    // Rule 1: Large amount
    if (amount > 10000)
        risk += 40;

    // Rule 2: Low remaining balance
    if ((balance - amount) < 500)
        risk += 20;

    // Rule 3: Large percentage of balance
    if (amount > 0.8 * balance)
        risk += 20;

    // Rule 4: New receiver (check in transaction history)
    ifstream file("transactions.csv");
    string line;
    bool known = false;

    while (getline(file, line)) {
        stringstream ss(line);
        string s, r;

        getline(ss, s, ',');
        getline(ss, r, ',');

        if (!s.empty() && stoi(s) == user->get_account_no() && stoi(r) == receiver) {
            known = true;
            break;
        }
    }

    if (!known)
        risk += 20;

    file.close();

    return risk;
}
// ---------------- MONEY TRANSFER ----------------
void send_money(Account* user, vector<Account> &accounts) {
    printHeader("TRANSFER FUNDS");

    int to_acc;
    double amt;

    cout << "Target Account No : ";
    cin >> to_acc;

    cout << "Transfer Amount $ : ";
    cin >> amt;

    if (amt <= 0) {
        cout << "\n[!] Invalid amount.\n";
    } 
    else {
        Account* receiver = find_account(accounts, to_acc);

        if (!receiver) {
            cout << "\n[!] Receiver not found.\n";
        } 
        else {
            // 🔥 FRAUD DETECTION (BEFORE ANY MONEY CHANGE)
            int risk = calculate_risk(user, to_acc, amt);

            cout << "\n[Risk Analysis]\n";
            cout << "Risk Score: " << risk << "%\n";

            if (risk >= 70) {
                cout << "[BLOCKED] High risk transaction.\n";
                pause();
                return;
            }
            else if (risk >= 40) {
                char choice;
                cout << "[WARNING] Suspicious transaction. Continue? (y/n): ";
                cin >> choice;

                if (choice != 'y' && choice != 'Y') {
                    cout << "\nTransaction cancelled.\n";
                    pause();
                    return;
                }
            }

            // 🔥 NOW SAFE TO PROCEED
            if (!user->withdraw(amt)) {
                cout << "\n[!] Insufficient balance.\n";
            } 
            else {
                receiver->deposit(amt);
                save_accounts(accounts);

                // Save transaction
                time_t now = time(0);

                ofstream file("transactions.csv", ios::app);
                file << user->get_account_no() << ","
                     << receiver->get_account_no() << ","
                     << amt << ",transfer,"
                     << now << "\n";

                file.close();

                cout << "\n[SUCCESS] Transfer complete!\n";
            }
        }
    }

    pause();
}
// ---------------- USER MENU ----------------
void user_menu(Account* user, vector<Account> &accounts) {
    int choice;

    do {
        string title = "WELCOME, " + user->get_name();
        printHeader(title);

        cout << "1. Check Balance\n";
        cout << "2. Deposit Money\n";
        cout << "3. Send Money\n";
        cout << "4. Transaction History\n";
        cout << "5. Logout\n";

        cout << "\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "\nBalance: $" << fixed << setprecision(2)
                 << user->get_balance() << endl;
            pause();
        }
        else if (choice == 2) {
            double amt;
            cout << "Deposit Amount: ";
            cin >> amt;

            if (amt > 0) {
                user->deposit(amt);
                save_accounts(accounts);
                cout << "\n[SUCCESS] Deposit done.\n";
            } else {
                cout << "\n[!] Invalid amount.\n";
            }

            pause();
        }
        else if (choice == 3) {
            send_money(user, accounts);
        }
        else if (choice == 4) {
            show_transaction_history(user);
        }

    } while (choice != 5);
}

// ---------------- MAIN ----------------
int main() {
    vector<Account> accounts = load_accounts();
    int choice;

    do {
        printHeader("SMART BANKING SYSTEM");

        cout << "1. Open Account\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";

        cout << "\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            create_account(accounts);
        }
        else if (choice == 2) {
            Account* user = login(accounts);
            if (user) user_menu(user, accounts);
        }

    } while (choice != 3);

    return 0;
}