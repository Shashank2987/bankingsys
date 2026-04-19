#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cctype>

using namespace std;

// --- CONSTANTS ---
const string SALT = "Smar7_B@nk_2024_#Secure!";
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";

// --- UTILITY FUNCTIONS ---
string to_string_safe(double val) {
    ostringstream oss;
    oss << fixed << setprecision(2) << val;
    return oss.str();
}

bool is_valid_double(const string& str, double& out) {
    try {
        size_t idx = 0;
        out = stod(str, &idx);
        return idx == str.length();
    } catch (...) {
        return false;
    }
}

bool is_valid_int(const string& str, int& out) {
    try {
        size_t idx = 0;
        out = stoi(str, &idx);
        return idx == str.length();
    } catch (...) {
        return false;
    }
}

string hash_password(const string &pass) {
    string salted_pass = SALT + pass + SALT;
    unsigned long hash = 5381;
    for (char c : salted_pass) {
        hash = ((hash << 5) + hash) + c;
    }
    return to_string(hash);
}

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
    cout << "\nPress Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}

// ============== ACCOUNT CLASS ==============
class Account {
private:
    int account_no;
    string name;
    string plain_password;
    string password_hash;
    double balance;
    bool active;

public:
    Account() : account_no(0), name(""), plain_password(""), password_hash(""), balance(0), active(true) {}
    
    Account(int acc, string n, string p, double b) {
        account_no = acc;
        name = n;
        plain_password = p;
        password_hash = hash_password(p);
        balance = b;
        active = true;
    }

    int get_account_no() const { return account_no; }
    string get_name() const { return name; }
    string get_plain_password() const { return plain_password; }
    string get_password_hash() const { return password_hash; }
    double get_balance() const { return balance; }
    bool is_active() const { return active; }
    
    void set_account_no(int acc) { account_no = acc; }
    void set_name(string n) { name = n; }
    void set_plain_password(string p) { plain_password = p; }
    void set_password_hash(string h) { password_hash = h; }
    void set_balance(double b) { balance = b; }
    void set_active(bool status) { active = status; }
    void deposit(double amt) { if (amt > 0) balance += amt; }
    bool withdraw(double amt) {
        if (amt > balance || amt <= 0) return false;
        balance -= amt;
        return true;
    }
};

// ============== LOAN STRUCTURE ==============
struct Loan {
    int account_no;
    string type;
    double principal;
    double interest_rate;
    int duration_months;
    int remaining_months;
    double monthly_emi;
    time_t applied_date;
    time_t last_emi_date;
    bool active;
};

// ============== FILE HANDLING ==============
vector<Account> load_accounts() {
    vector<Account> accounts;
    ifstream file("accounts.csv");

    if (!file.is_open()) {
        ofstream newfile("accounts.csv");
        newfile << "account_no,name,plain_password,password_hash,balance,active\n";
        newfile.close();
        return accounts;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;
        
        int commaCount = 0;
        vector<string> fields;
        string field;
        
        for (size_t i = 0; i < line.length(); i++) {
            if (line[i] == ',') {
                fields.push_back(field);
                field = "";
            } else {
                field += line[i];
            }
        }
        fields.push_back(field);
        
        if (fields.size() < 6) continue;

        int acc_no = 0;
        double bal = 0;
        
        if (!is_valid_int(fields[0], acc_no)) continue;
        if (!is_valid_double(fields[4], bal)) continue;

        Account a;
        a.set_account_no(acc_no);
        a.set_name(fields[1]);
        a.set_plain_password(fields[2]);
        a.set_password_hash(fields[3]);
        a.set_balance(bal);
        a.set_active(fields[5] == "1");
        
        accounts.push_back(a);
    }

    file.close();
    return accounts;
}

void save_accounts(vector<Account> &accounts) {
    ofstream file("accounts.csv");
    file << "account_no,name,plain_password,password_hash,balance,active\n";
    for (auto &acc : accounts) {
        file << acc.get_account_no() << ","
             << acc.get_name() << ","
             << acc.get_plain_password() << ","
             << acc.get_password_hash() << ","
             << fixed << setprecision(2) << acc.get_balance() << ","
             << (acc.is_active() ? 1 : 0) << "\n";
    }
    file.close();
}

void log_transaction(int from, int to, double amt, string type) {
    ofstream file("transactions.csv", ios::app);
    time_t now = time(0);
    file << from << "," << to << "," << fixed << setprecision(2) << amt << "," << type << "," << now << "\n";
    file.close();
}

vector<Loan> load_loans() {
    vector<Loan> loans;
    ifstream file("loans.csv");
    if (!file.is_open()) {
        ofstream newfile("loans.csv");
        newfile << "account_no,type,principal,interest_rate,duration_months,remaining_months,monthly_emi,applied_date,last_emi_date,active\n";
        newfile.close();
        return loans;
    }

    string line;
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        vector<string> fields;
        string field;
        
        for (size_t i = 0; i < line.length(); i++) {
            if (line[i] == ',') {
                fields.push_back(field);
                field = "";
            } else {
                field += line[i];
            }
        }
        fields.push_back(field);
        
        if (fields.size() < 10) continue;

        Loan l;
        int acc_no = 0;
        if (!is_valid_int(fields[0], acc_no)) continue;
        
        l.account_no = acc_no;
        l.type = fields[1];
        
        double principal = 0, interest_rate = 0, monthly_emi = 0;
        int duration = 0, remaining = 0;
        
        if (!is_valid_double(fields[2], principal)) continue;
        if (!is_valid_double(fields[3], interest_rate)) continue;
        if (!is_valid_int(fields[4], duration)) continue;
        if (!is_valid_int(fields[5], remaining)) continue;
        if (!is_valid_double(fields[6], monthly_emi)) continue;
        
        l.principal = principal;
        l.interest_rate = interest_rate;
        l.duration_months = duration;
        l.remaining_months = remaining;
        l.monthly_emi = monthly_emi;
        l.applied_date = (time_t)stol(fields[7]);
        l.last_emi_date = (time_t)stol(fields[8]);
        l.active = (fields[9] == "1");
        
        loans.push_back(l);
    }
    file.close();
    return loans;
}

void save_loans(vector<Loan> &loans) {
    ofstream file("loans.csv");
    file << "account_no,type,principal,interest_rate,duration_months,remaining_months,monthly_emi,applied_date,last_emi_date,active\n";
    for (auto &l : loans) {
        file << l.account_no << "," << l.type << "," << fixed << setprecision(2) << l.principal << ","
             << l.interest_rate << "," << l.duration_months << "," << l.remaining_months << ","
             << l.monthly_emi << "," << l.applied_date << "," << l.last_emi_date << ","
             << (l.active ? 1 : 0) << "\n";
    }
    file.close();
}

// ============== SYSTEM FUNCTIONS ==============
int generate_unique_account_number(vector<Account> &accounts) {
    int new_account_no = 100001;
    for (auto &acc : accounts) {
        if (acc.get_account_no() >= new_account_no) {
            new_account_no = acc.get_account_no() + 1;
        }
    }
    return new_account_no;
}

Account* find_account(vector<Account> &accounts, int acc_no) {
    for (auto &acc : accounts) {
        if (acc.get_account_no() == acc_no) {
            return &acc;
        }
    }
    return nullptr;
}

// ============== ACCOUNT CREATION ==============
void create_account(vector<Account> &accounts) {
    printHeader("OPEN NEW ACCOUNT");

    string name, pass;
    double bal = 0;

    int acc = generate_unique_account_number(accounts);
    
    cout << "Enter Name        : ";
    getline(cin, name);

    cout << "Enter Password    : ";
    getline(cin, pass);

    cout << "Initial Deposit Rs : ";
    string bal_str;
    getline(cin, bal_str);
    
    if (!is_valid_double(bal_str, bal)) {
        cout << "\n[ERROR] Invalid amount.\n";
        pause();
        return;
    }

    if (bal < 0) {
        cout << "\n[ERROR] Invalid amount (negative).\n";
        pause();
        return;
    }

    accounts.push_back(Account(acc, name, pass, bal));
    save_accounts(accounts);

    cout << "\n[SUCCESS] Account created!\n";
    cout << "[INFO] Your Account Number : " << acc << "\n";
    cout << "[INFO] Account Name        : " << name << "\n";
    cout << "[INFO] Password            : " << pass << "\n";
    cout << "[INFO] Initial Balance     : Rs" << fixed << setprecision(2) << bal << "\n";
    pause();
}

// ============== LOGIN ==============
Account* login(vector<Account> &accounts) {
    printHeader("USER LOGIN");

    int acc = 0;
    string acc_str, pass;

    cout << "Account Number : ";
    getline(cin, acc_str);
    
    if (!is_valid_int(acc_str, acc)) {
        cout << "\n[ERROR] Invalid account number.\n";
        pause();
        return nullptr;
    }

    cout << "Password       : ";
    getline(cin, pass);

    Account* user = find_account(accounts, acc);
    if (!user) {
        cout << "\n[ERROR] Account not found.\n";
        pause();
        return nullptr;
    }

    if (!user->is_active()) {
        cout << "\n[ERROR] Account has been terminated.\n";
        pause();
        return nullptr;
    }

    if (user->get_password_hash() != hash_password(pass)) {
        cout << "\n[ERROR] Wrong password.\n";
        pause();
        return nullptr;
    }

    cout << "\n[SUCCESS] Login successful.\n";
    pause();
    return user;
}

// ============== USER MENU ==============
void user_menu(Account* user, vector<Account> &accounts) {
    int choice = 0;

    do {
        Account* fresh_user = find_account(accounts, user->get_account_no());
        if (!fresh_user || !fresh_user->is_active()) {
            cout << "\n[ERROR] Your account has been terminated!\n";
            pause();
            break;
        }
        user = fresh_user;

        printHeader("WELCOME, " + user->get_name());

        cout << "1. Check Balance\n";
        cout << "2. Deposit Money\n";
        cout << "3. Logout\n";

        cout << "\nChoice: ";
        string choice_str;
        getline(cin, choice_str);
        
        if (!is_valid_int(choice_str, choice)) {
            cout << "[ERROR] Invalid input.\n";
            pause();
            continue;
        }

        if (choice == 1) {
            cout << "\nBalance: Rs" << fixed << setprecision(2) << user->get_balance() << "\n";
            pause();
        }
        else if (choice == 2) {
            cout << "Deposit Amount: ";
            string amt_str;
            getline(cin, amt_str);
            
            double amt = 0;
            if (!is_valid_double(amt_str, amt)) {
                cout << "\n[ERROR] Invalid amount.\n";
                pause();
                continue;
            }
            
            if (amt > 0) {
                user->deposit(amt);
                save_accounts(accounts);
                log_transaction(user->get_account_no(), user->get_account_no(), amt, "deposit");
                cout << "\n[SUCCESS] Deposit done.\n";
            } else {
                cout << "\n[ERROR] Invalid amount.\n";
            }
            pause();
        }

    } while (choice != 3);
}

// ============== ADMIN LOGIN ==============
bool admin_login() {
    printHeader("ADMIN LOGIN");
    
    string username, password;
    
    cout << "Admin Username : ";
    getline(cin, username);
    
    cout << "Admin Password : ";
    getline(cin, password);
    
    if (username != ADMIN_USERNAME) {
        cout << "\n[ERROR] Invalid admin username.\n";
        pause();
        return false;
    }
    
    if (password != ADMIN_PASSWORD) {
        cout << "\n[ERROR] Invalid admin password.\n";
        pause();
        return false;
    }
    
    cout << "\n[SUCCESS] Admin login successful.\n";
    pause();
    return true;
}

// ============== ADMIN FUNCTIONS ==============
void view_all_users(vector<Account> &accounts) {
    printHeader("ALL REGISTERED USERS");
    
    if (accounts.empty()) {
        cout << "No users registered.\n";
        pause();
        return;
    }
    
    cout << "\n";
    cout << left << setw(12) << "Account No" 
         << setw(20) << "Name" 
         << setw(20) << "Password"
         << setw(15) << "Balance"
         << setw(12) << "Status\n";
    cout << string(79, '=') << "\n";
    
    for (auto &acc : accounts) {
        cout << left << setw(12) << acc.get_account_no() 
             << setw(20) << acc.get_name()
             << setw(20) << acc.get_plain_password()
             << setw(15) << ("Rs" + to_string_safe(acc.get_balance()))
             << setw(12) << (acc.is_active() ? "ACTIVE" : "TERMINATED")
             << "\n";
    }
    cout << string(79, '=') << "\n";
    cout << "\nTotal Users: " << accounts.size() << "\n";
    pause();
}

void view_user_details(vector<Account> &accounts) {
    printHeader("VIEW USER DETAILS");
    
    cout << "Enter Account Number: ";
    string acc_str;
    getline(cin, acc_str);
    
    int acc_no = 0;
    if (!is_valid_int(acc_str, acc_no)) {
        cout << "\n[ERROR] Invalid account number.\n";
        pause();
        return;
    }
    
    Account* user = find_account(accounts, acc_no);
    if (!user) {
        cout << "\n[ERROR] User not found.\n";
        pause();
        return;
    }
    
    cout << "\n" << string(60, '=') << "\n";
    cout << "[USER DETAILS]\n";
    cout << string(60, '=') << "\n";
    cout << "Account Number  : " << user->get_account_no() << "\n";
    cout << "Name            : " << user->get_name() << "\n";
    cout << "Password        : " << user->get_plain_password() << "\n";
    cout << "Password Hash   : " << user->get_password_hash() << "\n";
    cout << "Current Balance : Rs" << fixed << setprecision(2) << user->get_balance() << "\n";
    cout << "Account Status  : " << (user->is_active() ? "ACTIVE" : "TERMINATED") << "\n";
    cout << string(60, '=') << "\n";
    pause();
}

void view_system_statistics(vector<Account> &accounts) {
    printHeader("SYSTEM STATISTICS");
    
    int total_accounts = 0;
    int active_accounts = 0;
    int terminated_accounts = 0;
    double total_balance = 0;

    for (auto &acc : accounts) {
        total_accounts++;
        total_balance += acc.get_balance();
        if (acc.is_active()) {
            active_accounts++;
        } else {
            terminated_accounts++;
        }
    }

    cout << "\n" << string(60, '=') << "\n";
    cout << "[ACCOUNT STATISTICS]\n";
    cout << string(60, '-') << "\n";
    cout << "Total Accounts          : " << total_accounts << "\n";
    cout << "Active Accounts         : " << active_accounts << "\n";
    cout << "Terminated Accounts     : " << terminated_accounts << "\n";
    cout << "Total System Balance    : Rs" << fixed << setprecision(2) << total_balance << "\n";
    cout << string(60, '=') << "\n";
    pause();
}

void terminate_account_admin(vector<Account> &accounts) {
    printHeader("TERMINATE USER ACCOUNT");
    
    cout << "Enter Account Number to Terminate: ";
    string acc_str;
    getline(cin, acc_str);
    
    int acc_no = 0;
    if (!is_valid_int(acc_str, acc_no)) {
        cout << "\n[ERROR] Invalid account number.\n";
        pause();
        return;
    }

    Account* user = find_account(accounts, acc_no);
    if (!user) {
        cout << "\n[ERROR] User not found.\n";
        pause();
        return;
    }

    if (!user->is_active()) {
        cout << "\n[ERROR] Account is already terminated.\n";
        pause();
        return;
    }

    cout << "\n[WARNING] Terminate account: " << user->get_name() << " (" << acc_no << ")\n";
    cout << "This action cannot be undone.\n";
    cout << "Are you sure? (yes/no): ";
    string confirm;
    getline(cin, confirm);

    if (confirm == "yes" || confirm == "YES") {
        user->set_active(false);
        save_accounts(accounts);
        log_transaction(acc_no, 0, 0, "admin_termination");
        cout << "\n[SUCCESS] Account terminated.\n";
    } else {
        cout << "\nTermination cancelled.\n";
    }
    pause();
}

void admin_menu(vector<Account> &accounts) {
    int choice = 0;
    
    do {
        printHeader("ADMIN DASHBOARD");
        
        cout << "1. View All Users\n";
        cout << "2. View User Details\n";
        cout << "3. View System Statistics\n";
        cout << "4. Terminate Account\n";
        cout << "5. Logout\n";
        
        cout << "\nChoice: ";
        string choice_str;
        getline(cin, choice_str);
        
        if (!is_valid_int(choice_str, choice)) {
            cout << "[ERROR] Invalid input.\n";
            pause();
            continue;
        }
        
        if (choice == 1) {
            view_all_users(accounts);
        }
        else if (choice == 2) {
            view_user_details(accounts);
        }
        else if (choice == 3) {
            view_system_statistics(accounts);
        }
        else if (choice == 4) {
            terminate_account_admin(accounts);
        }
        
    } while (choice != 5);
}

// ============== MAIN ==============
int main() {
    vector<Account> accounts = load_accounts();
    int choice = 0;

    do {
        printHeader("SMART BANKING SYSTEM");

        cout << "1. Open Account\n";
        cout << "2. User Login\n";
        cout << "3. Admin Login\n";
        cout << "4. Exit\n";

        cout << "\nChoice: ";
        string choice_str;
        getline(cin, choice_str);
        
        if (!is_valid_int(choice_str, choice)) {
            cout << "[ERROR] Invalid input.\n";
            pause();
            continue;
        }

        if (choice == 1) {
            create_account(accounts);
        }
        else if (choice == 2) {
            Account* user = login(accounts);
            if (user) user_menu(user, accounts);
        }
        else if (choice == 3) {
            if (admin_login()) {
                admin_menu(accounts);
            }
        }

    } while (choice != 4);

    cout << "\n[INFO] Thank you for using Smart Banking System!\n";
    return 0;
}