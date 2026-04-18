#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <algorithm>

using namespace std;

// --- CONSTANTS ---
const string SALT = "Smar7_B@nk_2024_#Secure!"; // Salting string

// --- Hashing Algorithm with Salting ---
string hash_password(const string &pass) {
    // 1st Task: Added Salting
    string salted_pass = SALT + pass + SALT; 
    unsigned long hash = 5381;

    for (char c : salted_pass) {
        hash = ((hash << 5) + hash) + c; // djb2 algorithm
    }

    return to_string(hash);
}

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
    cin.ignore(10000, '\n'); // Clear any leftover input
    cin.get();
}

// ============== LOAN STRUCTURE (IMPROVED) ==============
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

// ============== RECURRING TRANSACTION ==============
struct Recurring {
    int from_acc;
    int to_acc;
    double amount;
    int interval_days;
    time_t last_run;
};

// ============== ACCOUNT CLASS ==============
class Account {
private:
    int account_no;
    string name;
    string password;
    double balance;
    bool active;

public:
    Account() : active(true) {}
    Account(int acc, string n, string p, double b) {
        account_no = acc;
        name = n;
        password = p;
        balance = b;
        active = true;
    }

    int get_account_no() { return account_no; }
    string get_name() { return name; }
    string get_password() { return password; }
    double get_balance() { return balance; }
    bool is_active() { return active; }
    
    void set_active(bool status) { active = status; }

    void deposit(double amt) { 
        if (amt > 0) balance += amt; 
    }
    
    bool withdraw(double amt) {
        if (amt > balance || amt <= 0) return false;
        balance -= amt;
        return true;
    }
};

// ============== FILE HANDLING ============

vector<Account> load_accounts() {
    vector<Account> accounts;
    ifstream file("accounts.csv");

    if (!file.is_open()) {
        ofstream newfile("accounts.csv");
        newfile << "account_no,name,password,balance,active\n";
        newfile.close();
        return accounts;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string acc, name, pass, bal, active;

        getline(ss, acc, ',');
        getline(ss, name, ',');
        getline(ss, pass, ',');
        getline(ss, bal, ',');
        getline(ss, active, ',');

        if (!acc.empty()) {
            Account a(stoi(acc), name, pass, stod(bal));
            if (!active.empty()) a.set_active(stoi(active) == 1);
            accounts.push_back(a);
        }
    }

    file.close();
    return accounts;
}

void save_accounts(vector<Account> &accounts) {
    ofstream file("accounts.csv");
    file << "account_no,name,password,balance,active\n";
    for (auto &acc : accounts) {
        file << acc.get_account_no() << ","
             << acc.get_name() << ","
             << acc.get_password() << ","
             << fixed << setprecision(2) << acc.get_balance() << ","
             << (acc.is_active() ? 1 : 0) << "\n";
    }
    file.close();
}

// ============== LOAN FILE HANDLING ==============
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
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string acc, type, p, ir, d, rem, emi, ad, led, act;
        
        getline(ss, acc, ','); getline(ss, type, ','); getline(ss, p, ',');
        getline(ss, ir, ','); getline(ss, d, ','); getline(ss, rem, ',');
        getline(ss, emi, ','); getline(ss, ad, ','); getline(ss, led, ',');
        getline(ss, act, ',');
        
        if (!acc.empty()) {
            Loan l;
            l.account_no = stoi(acc);
            l.type = type;
            l.principal = stod(p);
            l.interest_rate = stod(ir);
            l.duration_months = stoi(d);
            l.remaining_months = stoi(rem);
            l.monthly_emi = stod(emi);
            l.applied_date = (time_t)stol(ad);
            l.last_emi_date = (time_t)stol(led);
            l.active = stoi(act) == 1;
            loans.push_back(l);
        }
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

vector<Recurring> load_recurring() {
    vector<Recurring> list;
    ifstream file("recurring.csv");
    if (!file.is_open()) {
        ofstream newfile("recurring.csv");
        newfile << "from_acc,to_acc,amount,interval_days,last_run\n";
        newfile.close();
        return list;
    }
    string line;
    getline(file, line); // Skip header
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string f, t, a, i, lr;
        getline(ss, f, ','); getline(ss, t, ','); getline(ss, a, ','); 
        getline(ss, i, ','); getline(ss, lr, ',');
        if (!f.empty()) 
            list.push_back({stoi(f), stoi(t), stod(a), stoi(i), (time_t)stol(lr)});
    }
    file.close();
    return list;
}

void save_all_recurring(vector<Recurring> &list) {
    ofstream file("recurring.csv");
    file << "from_acc,to_acc,amount,interval_days,last_run\n";
    for (auto &r : list) {
        file << r.from_acc << "," << r.to_acc << "," << fixed << setprecision(2) << r.amount << "," 
             << r.interval_days << "," << r.last_run << "\n";
    }
    file.close();
}

void log_transaction(int from, int to, double amt, string type, int acc_status = 1) {
    ofstream file("transactions.csv", ios::app);
    time_t now = time(0);
    file << from << "," << to << "," << fixed << setprecision(2) << amt << "," << type << "," << now << "," << acc_status << "\n";
    file.close();
}

// ============== SYSTEM FUNCTIONS ==============
int generate_unique_account_number(vector<Account> &accounts) {
    int new_account_no = 100001; // Start from 100001
    
    // Find the highest account number in use
    for (auto &acc : accounts) {
        if (acc.get_account_no() >= new_account_no) {
            new_account_no = acc.get_account_no() + 1;
        }
    }
    
    return new_account_no;
}

Account* find_account(vector<Account> &accounts, int acc_no) {
    for (auto &acc : accounts) {
        if (acc.get_account_no() == acc_no)
            return &acc;
    }
    return nullptr;
}

void create_account(vector<Account> &accounts) {
    printHeader("OPEN NEW ACCOUNT");

    string name, pass;
    double bal;

    // Auto-generate unique account number
    int acc = generate_unique_account_number(accounts);
    
    cout << "Enter Name        : ";
    getline(cin, name);

    cout << "Enter Password    : ";
    cin >> pass;
    cin.ignore(10000, '\n');
    
    // Hash the password with salt before storing
    pass = hash_password(pass);

    cout << "Initial Deposit Rs : ";
    cin >> bal;
    cin.ignore(10000, '\n');

    if (bal < 0) {
        cout << "\n[!] Invalid amount.\n";
        pause();
        return;
    }

    accounts.push_back(Account(acc, name, pass, bal));
    save_accounts(accounts);

    cout << "\n[SUCCESS] Account created!\n";
    cout << "[INFO] Your Account Number: " << acc << "\n";
    cout << "[INFO] Account Name: " << name << "\n";
    cout << "[INFO] Initial Balance: Rs" << fixed << setprecision(2) << bal << "\n";
    pause();
}

// ============== EMI PROCESSING ENGINE ==============
void process_emi_payments(vector<Account> &accounts) {
    vector<Loan> loans = load_loans();
    vector<Account> deleted_accounts;
    bool changed = false;
    time_t now = time(0);

    for (auto &loan : loans) {
        if (!loan.active) continue;

        Account* borrower = find_account(accounts, loan.account_no);
        if (!borrower) continue;

        // Check if 30 days have passed since last EMI
        double diff_seconds = difftime(now, loan.last_emi_date);
        double month_seconds = 30 * 86400; // Approximate month in seconds

        if (diff_seconds >= month_seconds) {
            if (loan.remaining_months > 0) {
                // Try to deduct EMI
                if (borrower->withdraw(loan.monthly_emi)) {
                    loan.remaining_months--;
                    loan.last_emi_date = now;
                    changed = true;

                    log_transaction(loan.account_no, 0, loan.monthly_emi, "EMI", 1);
                    
                    cout << "[SYSTEM] EMI of Rs" << fixed << setprecision(2) << loan.monthly_emi 
                         << " deducted from Account " << loan.account_no << "\n";

                    // Check if loan is paid off
                    if (loan.remaining_months <= 0) {
                        loan.active = false;
                        cout << "[INFO] Loan fully paid! Account " << loan.account_no << " loan closed.\n";
                    }
                } else {
                    // INSUFFICIENT FUNDS - ACCOUNT TERMINATED
                    cout << "\n[CRITICAL] Account " << loan.account_no << " (" << borrower->get_name() 
                         << ") - Insufficient funds for EMI payment of Rs" << fixed << setprecision(2) 
                         << loan.monthly_emi << "\n";
                    cout << "[ACTION] Account has been TERMINATED and DELETED.\n\n";

                    borrower->set_active(false);
                    loan.active = false;
                    changed = true;

                    log_transaction(loan.account_no, 0, loan.monthly_emi, "EMI_FAILED_TERMINATED", 0);
                }
            }
        }
    }

    if (changed) {
        save_accounts(accounts);
        save_loans(loans);
    }
}

// ============== RECURRING ENGINE ==============
void process_recurring_logic(Account* user, vector<Account> &accounts) {
    vector<Recurring> recs = load_recurring();
    bool changed = false;
    time_t now = time(0);

    for (auto &r : recs) {
        if (r.from_acc == user->get_account_no()) {
            double diff_seconds = difftime(now, r.last_run);
            double interval_seconds = r.interval_days * 86400;

            if (diff_seconds >= interval_seconds) {
                Account* receiver = find_account(accounts, r.to_acc);
                if (receiver && user->withdraw(r.amount)) {
                    receiver->deposit(r.amount);
                    r.last_run = now;
                    changed = true;

                    log_transaction(r.from_acc, r.to_acc, r.amount, "recurring", 1);
                    
                    cout << "\n[SYSTEM] Recurring payment of Rs" << fixed << setprecision(2) 
                         << r.amount << " to Account " << r.to_acc << " processed.\n";
                } else if (!receiver) {
                    cout << "\n[WARNING] Recurring payment to Account " << r.to_acc << " failed - recipient not found.\n";
                } else {
                    cout << "\n[WARNING] Recurring payment failed - insufficient balance.\n";
                }
            }
        }
    }
    if (changed) {
        save_accounts(accounts);
        save_all_recurring(recs);
    }
}

// ============== LOGIN ==============
Account* login(vector<Account> &accounts) {
    printHeader("LOGIN");

    int acc;
    string pass;

    cout << "Account No : ";
    cin >> acc;
    cin.ignore(10000, '\n'); // Clear input buffer
    
    cout << "Password   : ";
    getline(cin, pass); // Use getline for password to handle spaces
    
    Account* user = find_account(accounts, acc);
    if (!user) {
        cout << "\n[!] Account not found.\n";
        pause();
        return nullptr;
    }

    if (!user->is_active()) {
        cout << "\n[!] Account has been terminated.\n";
        pause();
        return nullptr;
    }

    if (user->get_password() != hash_password(pass)) {
        cout << "\n[!] Wrong password.\n";
        pause();
        return nullptr;
    }

    cout << "\n[SUCCESS] Login successful.\n";
    pause();

    // Process EMI and recurring before showing user menu
    process_emi_payments(accounts);
    process_recurring_logic(user, accounts);

    return user;
}

void show_transaction_history(Account* user) {
    printHeader("TRANSACTION HISTORY");
    
    ifstream file("transactions.csv");
    if (!file.is_open()) {
        cout << "No transactions found.\n";
        pause();
        return;
    }

    string line;
    bool found = false;
    int count = 0;

    while (getline(file, line) && count < 20) {
        if (line.empty()) continue;
        stringstream ss(line);
        string from, to, amt, type, timestamp, status;
        
        getline(ss, from, ',');
        getline(ss, to, ',');
        getline(ss, amt, ',');
        getline(ss, type, ',');
        getline(ss, timestamp, ',');
        getline(ss, status, ',');

        if (!from.empty() && stoi(from) == user->get_account_no()) {
            found = true;
            cout << "[" << type << "] Amount: Rs" << amt 
                 << " | To: " << to << " | Time: " << timestamp << "\n";
            count++;
        }
    }

    if (!found) cout << "No transactions found for this account.\n";
    file.close();
    pause();
}

void generate_statement(Account* user) {
    printHeader("BANK STATEMENT");

    cout << "\nAccount No : " << user->get_account_no() << "\n";
    cout << "Name       : " << user->get_name() << "\n";
    cout << "Balance    : Rs" << fixed << setprecision(2) << user->get_balance() << "\n";
    cout << "Status     : " << (user->is_active() ? "ACTIVE" : "TERMINATED") << "\n";

    cout << "\n--- Recent Transactions ---\n";
    show_transaction_history(user);
}

int calculate_risk(Account* user, int receiver, double amount) {
    int risk = 0;
    double balance = user->get_balance();

    if (amount > 100000) risk += 30;
    if (amount > 10000) risk += 40;
    if ((balance - amount) < 500) risk += 20;
    if (amount > 0.8 * balance) risk += 20;

    ifstream file("transactions.csv");
    string line;
    bool known = false;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string s, r;
        getline(ss, s, ',');
        getline(ss, r, ',');
        if (s.empty() || r.empty()) continue;
        try {
            if (stoi(s) == user->get_account_no() && stoi(r) == receiver) {
                known = true;
                break;
            }
        } catch (...) {
            continue;
        }
    }
    if (!known) risk += 20;
    file.close();
    return risk;
}

// ============== MONEY TRANSFER ==============
void send_money(Account* user, vector<Account> &accounts) {
    printHeader("TRANSFER FUNDS");
    int to_acc;
    double amt;
    
    cout << "Target Account No : ";
    cin >> to_acc;
    cin.ignore(10000, '\n');
    
    cout << "Transfer Amount Rs : ";
    cin >> amt;
    cin.ignore(10000, '\n');

    if (amt <= 0) {
        cout << "\n[!] Invalid amount.\n";
        pause();
        return;
    }

    Account* receiver = find_account(accounts, to_acc);
    if (!receiver) {
        cout << "\n[!] Receiver not found.\n";
        pause();
        return;
    }

    if (!receiver->is_active()) {
        cout << "\n[!] Receiver account has been terminated.\n";
        pause();
        return;
    }

    int risk = calculate_risk(user, to_acc, amt);
    cout << "\n[Risk Analysis]\nRisk Score: " << risk << "%\n";
    
    if (risk >= 70) {
        cout << "[BLOCKED] High risk transaction.\n";
        pause();
        return;
    } else if (risk >= 40) {
        char choice;
        cout << "[WARNING] Suspicious transaction. Continue? (y/n): ";
        cin >> choice;
        cin.ignore(10000, '\n');
        if (choice != 'y' && choice != 'Y') {
            cout << "\nTransaction cancelled.\n";
            pause();
            return;
        }
    }

    if (!user->withdraw(amt)) {
        cout << "\n[!] Insufficient balance.\n";
    } else {
        receiver->deposit(amt);
        save_accounts(accounts);
        log_transaction(user->get_account_no(), receiver->get_account_no(), amt, "transfer", 1);
        cout << "\n[SUCCESS] Transfer complete!\n";
    }
    pause();
}

// ============== LOAN SYSTEM ==============
void loan_menu(Account* user, vector<Account> &accounts) {
    printHeader("LOAN DEPARTMENT");
    cout << "1. Home Loan (8% Interest)\n";
    cout << "2. Car Loan (10% Interest)\n";
    cout << "3. Gold Loan (12% Interest)\n";
    cout << "4. Check Loan Status\n";
    cout << "5. Back\n";
    
    int choice;
    cout << "\nChoice: ";
    cin >> choice;
    cin.ignore(10000, '\n');
    
    if (choice >= 1 && choice <= 3) {
        Loan l;
        l.account_no = user->get_account_no();
        
        if (choice == 1) { l.type = "Home"; l.interest_rate = 8.0; }
        else if (choice == 2) { l.type = "Car"; l.interest_rate = 10.0; }
        else { l.type = "Gold"; l.interest_rate = 12.0; }
        
        cout << "Enter Principal Amount Rs: ";
        cin >> l.principal;
        cin.ignore(10000, '\n');
        
        cout << "Enter Duration (Months)  : ";
        cin >> l.duration_months;
        cin.ignore(10000, '\n');
        
        if (l.principal <= 0 || l.duration_months <= 0) {
            cout << "\n[!] Invalid amount or duration.\n";
            pause();
            return;
        }

        // Eligibility check: balance must be at least 10% of principal
        if (user->get_balance() < (l.principal * 0.1)) {
            cout << "\n[REJECTED] Insufficient collateral (Balance must be 10% of Loan).\n";
            pause();
            return;
        }

        // Calculate EMI using standard formula
        double monthly_rate = l.interest_rate / 100.0 / 12.0;
        l.monthly_emi = (l.principal * monthly_rate) / 
                        (1 - pow(1 + monthly_rate, -l.duration_months));

        l.applied_date = time(0);
        l.last_emi_date = time(0);
        l.remaining_months = l.duration_months;
        l.active = true;

        // Save loan details
        vector<Loan> loans = load_loans();
        loans.push_back(l);
        save_loans(loans);

        // Disburse the loan amount to account
        user->deposit(l.principal);
        save_accounts(accounts);

        log_transaction(0, l.account_no, l.principal, "loan_disbursed", 1);

        cout << "\n[SUCCESS] Loan approved and Rs" << fixed << setprecision(2)
             << l.principal << " credited to your account.\n";
        cout << "[INFO]    Estimated EMI: Rs" << fixed << setprecision(2)
             << l.monthly_emi << " / month for " << l.duration_months << " months.\n";
        cout << "[WARNING] EMI will be deducted every 30 days. Ensure sufficient balance!\n";
    } 
    else if (choice == 4) {
        vector<Loan> loans = load_loans();
        bool found = false;
        
        for (auto &l : loans) {
            if (l.account_no == user->get_account_no()) {
                found = true;
                cout << "\n[" << l.type << " Loan]\n";
                cout << "  Status       : " << (l.active ? "ACTIVE" : "CLOSED") << "\n";
                cout << "  Principal    : Rs" << fixed << setprecision(2) << l.principal << "\n";
                cout << "  Interest Rate: " << l.interest_rate << "%\n";
                cout << "  Monthly EMI  : Rs" << fixed << setprecision(2) << l.monthly_emi << "\n";
                cout << "  Remaining    : " << l.remaining_months << " months\n";
            }
        }
        if (!found) cout << "\nNo active loan applications found.\n";
    }
    pause();
}

// ============== RECURRING MANAGEMENT ==============
void recurring_menu(Account* user, vector<Account> &accounts) {
    printHeader("RECURRING PAYMENTS");
    cout << "1. Schedule New Recurring Payment\n";
    cout << "2. View My Scheduled Payments\n";
    cout << "3. Back\n";
    
    int choice;
    cout << "\nChoice: ";
    cin >> choice;
    cin.ignore(10000, '\n');
    
    if (choice == 1) {
        Recurring r;
        r.from_acc = user->get_account_no();
        
        cout << "Target Account No: ";
        cin >> r.to_acc;
        cin.ignore(10000, '\n');
        
        cout << "Amount Rs        : ";
        cin >> r.amount;
        cin.ignore(10000, '\n');
        
        cout << "Interval (Days)  : ";
        cin >> r.interval_days;
        cin.ignore(10000, '\n');
        
        if (r.amount <= 0 || r.interval_days <= 0) {
            cout << "\n[!] Invalid amount or interval.\n";
            pause();
            return;
        }

        if (!find_account(accounts, r.to_acc)) {
            cout << "\n[!] Target account not found.\n";
            pause();
            return;
        }

        r.last_run = time(0);
        vector<Recurring> list = load_recurring();
        list.push_back(r);
        save_all_recurring(list);

        cout << "\n[SUCCESS] Recurring payment scheduled.\n";
    } 
    else if (choice == 2) {
        vector<Recurring> list = load_recurring();
        bool found = false;
        
        for (auto &r : list) {
            if (r.from_acc == user->get_account_no()) {
                found = true;
                cout << "To Account: " << r.to_acc << " | Amount: Rs" << fixed << setprecision(2) 
                     << r.amount << " | Every " << r.interval_days << " days\n";
            }
        }
        if (!found) cout << "No recurring payments found.\n";
    }
    pause();
}

// ============== USER MENU ==============
void user_menu(Account* user, vector<Account> &accounts) {
    int choice;

    do {
        // Reload account to get fresh data
        Account* fresh_user = find_account(accounts, user->get_account_no());
        if (!fresh_user || !fresh_user->is_active()) {
            cout << "\n[ERROR] Your account has been terminated!\n";
            pause();
            break;
        }
        user = fresh_user;

        string title = "WELCOME, " + user->get_name();
        printHeader(title);

        cout << "1. Check Balance\n";
        cout << "2. Deposit Money\n";
        cout << "3. Send Money\n";
        cout << "4. Loans (Home/Car/Gold)\n";
        cout << "5. Recurring Transactions\n";
        cout << "6. Transaction History\n";
        cout << "7. Generate Bank Statement\n";
        cout << "8. Logout\n";

        cout << "\nChoice: ";
        cin >> choice;
        cin.ignore(10000, '\n');

        if (choice == 1) {
            cout << "\nBalance: Rs" << fixed << setprecision(2) << user->get_balance() << endl;
            pause();
        }
        else if (choice == 2) {
            double amt;
            cout << "Deposit Amount: ";
            cin >> amt;
            cin.ignore(10000, '\n');
            if (amt > 0) {
                user->deposit(amt);
                save_accounts(accounts);
                log_transaction(user->get_account_no(), user->get_account_no(), amt, "deposit", 1);
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
            loan_menu(user, accounts);
        }
        else if (choice == 5) {
            recurring_menu(user, accounts);
        }
        else if (choice == 6) {
            show_transaction_history(user);
        }
        else if (choice == 7) {
            generate_statement(user);
        }

    } while (choice != 8);
}

// ============== MAIN ==============
int main() {
    vector<Account> accounts = load_accounts();
    int choice;

    do {
        printHeader("SMART BANKING SYSTEM");

        cout << "1. Open a New Account \n";
        cout << "2. Login\n";
        cout << "3. Exit\n";

        cout << "\nChoice: ";
        cin >> choice;
        cin.ignore(10000, '\n');

        if (choice == 1) {
            create_account(accounts);
        }
        else if (choice == 2) {
            Account* user = login(accounts);
            if (user) user_menu(user, accounts);
        }

    } while (choice != 3);

    cout << "\n[INFO] Thank you for using Smart Banking System!\n";
    return 0;
}