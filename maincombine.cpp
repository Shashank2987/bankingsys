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
// Constants
const string SALT = "Smar7_B@nk_2024_#Secure!";
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";

// Utility Functions
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

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

// Credit Card Functions
bool validate_luhn(const string& cardNum) {
    if (cardNum.length() < 13 || cardNum.length() > 19) return false;
    
    int sum = 0;
    bool alternate = false;
    
    for (int i = cardNum.length() - 1; i >= 0; i--) {
        if (!isdigit(cardNum[i])) return false;
        
        int digit = cardNum[i] - '0';
        if (alternate) {
            digit *= 2;
            if (digit > 9) digit -= 9;
        }
        sum += digit;
        alternate = !alternate;
    }
    
    return sum % 10 == 0;
}

string get_card_type(const string& cardNum) {
    if (cardNum.length() < 2) return "UNKNOWN";
    
    if (cardNum[0] == '4' && (cardNum.length() == 13 || cardNum.length() == 16 || cardNum.length() == 19)) {
        return "VISA";
    }
    
    if (cardNum.length() >= 2) {
        int first_two = stoi(cardNum.substr(0, 2));
        if ((first_two >= 51 && first_two <= 55) && (cardNum.length() == 16)) {
            return "MASTERCARD";
        }
    }
    
    if (cardNum.length() >= 2) {
        int first_two = stoi(cardNum.substr(0, 2));
        if ((first_two == 34 || first_two == 37) && cardNum.length() == 15) {
            return "AMERICAN EXPRESS";
        }
    }
    
    if (cardNum.length() >= 4) {
        int first_four = stoi(cardNum.substr(0, 4));
        if ((first_four == 6011 || first_four == 6512 || first_four == 6556) && (cardNum.length() == 16)) {
            return "DISCOVER";
        }
    }
    
    if (cardNum.length() >= 4) {
        int first_four = stoi(cardNum.substr(0, 4));
        if (first_four >= 6070 && first_four <= 6074 && (cardNum.length() == 16)) {
            return "RUPAY";
        }
    }
    
    if (cardNum.length() >= 2) {
        int first_two = stoi(cardNum.substr(0, 2));
        if ((first_two == 36 || first_two == 38 || first_two == 39) && (cardNum.length() == 14)) {
            return "DINERS CLUB";
        }
    }
    
    if (cardNum.length() >= 4) {
        int first_four = stoi(cardNum.substr(0, 4));
        if ((first_four >= 3528 && first_four <= 3589) && (cardNum.length() == 16)) {
            return "JCB";
        }
    }
    
    return "UNKNOWN";
}

// UI Functions
void printBanner() {
    clearScreen();
    cout << string(80, '=') << "\n";
    cout << "              SMART BANKING SYSTEM \n";
    cout << string(80, '=') << "\n\n";
}

void printHeader(string title) {
    clearScreen();
    cout << "\n" << string(80, '-') << "\n";
    cout << "  " << title << "\n";
    cout << string(80, '-') << "\n\n";
}

void printSuccess(string msg) {
    cout << "  [+] " << msg << "\n";
}

void printError(string msg) {
    cout << "  [-] " << msg << "\n";
}

void printInfo(string msg) {
    cout << "  [i] " << msg << "\n";
}

void printWarning(string msg) {
    cout << "  [!] " << msg << "\n";
}

void printSection(string title) {
    cout << "\n  >>> " << title << "\n";
    cout << "      " << string(title.length(), '-') << "\n";
}

void printMenuItem(int num, string text) {
    cout << "  [" << num << "] " << text << "\n";
}

void printInput(string prompt) {
    cout << "  >> " << prompt;
}

void pauseProgram() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}

// Account Class
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

// Loan Structure
struct Loan {
    int account_no;
    string type;
    double principal;
    double interest_rate;
    int duration_months;
    int remaining_months;
    double monthly_emi;
    time_t applied_date;
    time_t last_payment_date;
    bool active;
};

// Recurring Payment Structure
struct RecurringPayment {
    int id;
    int from_account;
    int to_account;
    double amount;
    int interval_days;
    time_t last_payment;
    bool active;
};

// File Handling Functions
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

vector<Loan> load_loans() {
    vector<Loan> loans;
    ifstream file("loans.csv");
    if (!file.is_open()) {
        ofstream newfile("loans.csv");
        newfile << "account_no,type,principal,interest_rate,duration_months,remaining_months,monthly_emi,applied_date,last_payment_date,active\n";
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
        l.last_payment_date = (time_t)stol(fields[8]);
        l.active = (fields[9] == "1");
        
        loans.push_back(l);
    }
    file.close();
    return loans;
}

void save_loans(vector<Loan> &loans) {
    ofstream file("loans.csv");
    file << "account_no,type,principal,interest_rate,duration_months,remaining_months,monthly_emi,applied_date,last_payment_date,active\n";
    for (auto &l : loans) {
        file << l.account_no << "," << l.type << "," << fixed << setprecision(2) << l.principal << ","
             << l.interest_rate << "," << l.duration_months << "," << l.remaining_months << ","
             << l.monthly_emi << "," << l.applied_date << "," << l.last_payment_date << ","
             << (l.active ? 1 : 0) << "\n";
    }
    file.close();
}

vector<RecurringPayment> load_recurring() {
    vector<RecurringPayment> payments;
    ifstream file("recurring.csv");
    if (!file.is_open()) {
        ofstream newfile("recurring.csv");
        newfile << "id,from_account,to_account,amount,interval_days,last_payment,active\n";
        newfile.close();
        return payments;
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
        
        if (fields.size() < 7) continue;

        RecurringPayment r;
        if (!is_valid_int(fields[0], r.id)) continue;
        if (!is_valid_int(fields[1], r.from_account)) continue;
        if (!is_valid_int(fields[2], r.to_account)) continue;
        if (!is_valid_double(fields[3], r.amount)) continue;
        if (!is_valid_int(fields[4], r.interval_days)) continue;
        
        r.last_payment = (time_t)stol(fields[5]);
        r.active = (fields[6] == "1");
        
        payments.push_back(r);
    }
    file.close();
    return payments;
}

void save_recurring(vector<RecurringPayment> &payments) {
    ofstream file("recurring.csv");
    file << "id,from_account,to_account,amount,interval_days,last_payment,active\n";
    for (auto &r : payments) {
        file << r.id << "," << r.from_account << "," << r.to_account << ","
             << fixed << setprecision(2) << r.amount << "," << r.interval_days << ","
             << r.last_payment << "," << (r.active ? 1 : 0) << "\n";
    }
    file.close();
}

void log_transaction(int from, int to, double amt, string type, string description = "") {
    ofstream file("transactions.csv", ios::app);
    time_t now = time(0);
    file << from << "," << to << "," << fixed << setprecision(2) << amt << "," 
         << type << "," << now << "," << description << "\n";
    file.close();
}

// System Functions
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

int get_next_recurring_id(vector<RecurringPayment> &payments) {
    int max_id = 0;
    for (auto &p : payments) {
        if (p.id > max_id) max_id = p.id;
    }
    return max_id + 1;
}

// EMI Calculator
void calculate_emi(Account* user) {
    printHeader("EMI CALCULATOR");
    
    cout << "\n";
    double principal = 0, interest_rate = 0;
    int months = 0;
    
    printInput("Principal Amount (Rs): ");
    string principal_str;
    getline(cin, principal_str);
    
    if (!is_valid_double(principal_str, principal) || principal <= 0) {
        printError("Invalid principal amount!");
        pauseProgram();
        return;
    }
    
    printInput("Annual Interest Rate (%): ");
    string rate_str;
    getline(cin, rate_str);
    
    if (!is_valid_double(rate_str, interest_rate) || interest_rate < 0) {
        printError("Invalid interest rate!");
        pauseProgram();
        return;
    }
    
    printInput("Loan Duration (Months): ");
    string months_str;
    getline(cin, months_str);
    
    if (!is_valid_int(months_str, months) || months <= 0) {
        printError("Invalid duration!");
        pauseProgram();
        return;
    }
    
    double monthly_rate = interest_rate / 100.0 / 12.0;
    double emi;
    
    if (monthly_rate == 0) {
        emi = principal / months;
    } else {
        emi = (principal * monthly_rate * pow(1 + monthly_rate, months)) / 
              (pow(1 + monthly_rate, months) - 1);
    }
    
    double total_amount = emi * months;
    double total_interest = total_amount - principal;
    
    cout << "\n";
    printSection("EMI Calculation Results");
    printInfo("Principal Amount: Rs" + to_string_safe(principal));
    printInfo("Interest Rate: " + to_string_safe(interest_rate) + "%");
    printInfo("Loan Duration: " + to_string(months) + " months");
    printInfo("Monthly EMI: Rs" + to_string_safe(emi));
    printInfo("Total Interest: Rs" + to_string_safe(total_interest));
    printInfo("Total Amount Payable: Rs" + to_string_safe(total_amount));
    cout << "\n";
    
    cout << "  EMI BREAKDOWN:\n";
    cout << "  " << string(70, '-') << "\n";
    cout << "  Year | Monthly EMI | Total Paid | Principal | Interest\n";
    cout << "  " << string(70, '-') << "\n";
    
    double remaining_principal = principal;
    for (int year = 1; year <= (months + 11) / 12; year++) {
        int year_months = min(12, months - (year - 1) * 12);
        double year_emi = emi * year_months;
        double year_principal = 0;
        double year_interest = 0;
        
        for (int i = 0; i < year_months; i++) {
            double interest_paid = remaining_principal * monthly_rate;
            double principal_paid = emi - interest_paid;
            year_interest += interest_paid;
            year_principal += principal_paid;
            remaining_principal -= principal_paid;
        }
        
        cout << "  " << year << "    | " << setw(11) << fixed << setprecision(2) << emi 
             << " | " << setw(10) << year_emi << " | " << setw(9) << year_principal 
             << " | " << setw(8) << year_interest << "\n";
    }
    
    cout << "  " << string(70, '-') << "\n";
    cout << "\n";
    pauseProgram();
}

// Credit Card Validator
void validate_credit_card(Account* user) {
    printHeader("CREDIT CARD VALIDATOR");
    
    cout << "\n";
    printInput("Credit Card Number: ");
    string card_num;
    getline(cin, card_num);
    
    card_num.erase(remove(card_num.begin(), card_num.end(), ' '), card_num.end());
    
    if (card_num.length() < 13 || card_num.length() > 19) {
        printError("Invalid card number length!");
        cout << "\n";
        pauseProgram();
        return;
    }
    
    for (char c : card_num) {
        if (!isdigit(c)) {
            printError("Card number must contain only digits!");
            cout << "\n";
            pauseProgram();
            return;
        }
    }
    
    bool valid = validate_luhn(card_num);
    string card_type = get_card_type(card_num);
    
    cout << "\n";
    printSection("Card Validation Results");
    
    if (valid) {
        printSuccess("Card number is VALID!");
    } else {
        printError("Card number is INVALID!");
    }
    
    cout << "\n";
    printInfo("Card Type: " + card_type);
    printInfo("Card Length: " + to_string(card_num.length()) + " digits");
    printInfo("First 4 Digits: " + card_num.substr(0, 4));
    printInfo("Last 4 Digits: " + string(card_num.length() - 4, '*') + card_num.substr(card_num.length() - 4));
    
    cout << "\n";
    cout << "  CARD INFORMATION:\n";
    
    if (card_type == "VISA") {
        cout << "  - Starts with 4\n  - Length: 13, 16, or 19 digits\n";
    } else if (card_type == "MASTERCARD") {
        cout << "  - Starts with 51-55\n  - Length: 16 digits\n";
    } else if (card_type == "AMERICAN EXPRESS") {
        cout << "  - Starts with 34 or 37\n  - Length: 15 digits\n";
    } else if (card_type == "DISCOVER") {
        cout << "  - Starts with 6011, 6512, or 6556\n  - Length: 16 digits\n";
    } else if (card_type == "RUPAY") {
        cout << "  - Starts with 6070-6074\n  - Length: 16 digits\n";
    } else if (card_type == "DINERS CLUB") {
        cout << "  - Starts with 36, 38, or 39\n  - Length: 14 digits\n";
    } else if (card_type == "JCB") {
        cout << "  - Starts with 3528-3589\n  - Length: 16 digits\n";
    } else {
        cout << "  - Unknown card type\n";
    }
    
    cout << "\n";
    pauseProgram();
}

// Get Loan Function
void get_loan(Account* user, vector<Account> &accounts) {
    printHeader("GET A LOAN");
    
    cout << "\n";
    cout << "  Available Loan Types:\n";
    printMenuItem(1, "Home Loan (8% Interest)");
    printMenuItem(2, "Car Loan (10% Interest)");
    printMenuItem(3, "Personal Loan (12% Interest)");
    printMenuItem(4, "Student Loan (5% Interest)");
    printMenuItem(5, "Back");
    
    cout << "\n";
    printInput("Select loan type: ");
    string choice_str;
    getline(cin, choice_str);
    
    int choice = 0;
    if (!is_valid_int(choice_str, choice) || choice < 1 || choice > 5) {
        printError("Invalid choice!");
        pauseProgram();
        return;
    }
    
    if (choice == 5) return;
    
    Loan loan;
    loan.account_no = user->get_account_no();
    
    switch(choice) {
        case 1: loan.type = "Home"; loan.interest_rate = 8.0; break;
        case 2: loan.type = "Car"; loan.interest_rate = 10.0; break;
        case 3: loan.type = "Personal"; loan.interest_rate = 12.0; break;
        case 4: loan.type = "Student"; loan.interest_rate = 5.0; break;
    }
    
    cout << "\n";
    printInput("Principal Amount (Rs): ");
    string principal_str;
    getline(cin, principal_str);
    
    if (!is_valid_double(principal_str, loan.principal) || loan.principal <= 0) {
        printError("Invalid amount!");
        pauseProgram();
        return;
    }
    
    printInput("Loan Duration (Months): ");
    string duration_str;
    getline(cin, duration_str);
    
    if (!is_valid_int(duration_str, loan.duration_months) || loan.duration_months <= 0) {
        printError("Invalid duration!");
        pauseProgram();
        return;
    }
    
    // Check eligibility
    if (user->get_balance() < (loan.principal * 0.1)) {
        printError("Insufficient collateral! Balance must be at least 10% of loan amount.");
        pauseProgram();
        return;
    }
    
    // Calculate EMI
    double monthly_rate = loan.interest_rate / 100.0 / 12.0;
    loan.monthly_emi = (loan.principal * monthly_rate * pow(1 + monthly_rate, loan.duration_months)) / 
                       (pow(1 + monthly_rate, loan.duration_months) - 1);
    
    loan.applied_date = time(0);
    loan.last_payment_date = time(0);
    loan.remaining_months = loan.duration_months;
    loan.active = true;
    
    vector<Loan> loans = load_loans();
    loans.push_back(loan);
    save_loans(loans);
    
    // Disburse loan amount
    user->deposit(loan.principal);
    save_accounts(accounts);
    
    log_transaction(0, user->get_account_no(), loan.principal, "LOAN_DISBURSED", 
                   loan.type + " Loan - Rs" + to_string_safe(loan.principal));
    
    cout << "\n";
    printSuccess("Loan approved and disbursed!");
    printSection("Loan Details");
    printInfo("Loan Type: " + loan.type);
    printInfo("Principal: Rs" + to_string_safe(loan.principal));
    printInfo("Interest Rate: " + to_string_safe(loan.interest_rate) + "%");
    printInfo("Monthly EMI: Rs" + to_string_safe(loan.monthly_emi));
    printInfo("Duration: " + to_string(loan.duration_months) + " months");
    printInfo("Amount Disbursed: Rs" + to_string_safe(loan.principal));
    cout << "\n";
    pauseProgram();
}

// Transfer Money Function
void transfer_money(Account* user, vector<Account> &accounts) {
    printHeader("TRANSFER MONEY");
    
    cout << "\n";
    int to_acc = 0;
    double amount = 0;
    
    printInput("Recipient Account Number: ");
    string to_acc_str;
    getline(cin, to_acc_str);
    
    if (!is_valid_int(to_acc_str, to_acc)) {
        printError("Invalid account number!");
        pauseProgram();
        return;
    }
    
    Account* recipient = find_account(accounts, to_acc);
    if (!recipient) {
        printError("Account not found!");
        pauseProgram();
        return;
    }
    
    if (!recipient->is_active()) {
        printError("Recipient account is terminated!");
        pauseProgram();
        return;
    }
    
    printInput("Transfer Amount (Rs): ");
    string amount_str;
    getline(cin, amount_str);
    
    if (!is_valid_double(amount_str, amount) || amount <= 0) {
        printError("Invalid amount!");
        pauseProgram();
        return;
    }
    
    if (!user->withdraw(amount)) {
        printError("Insufficient balance!");
        pauseProgram();
        return;
    }
    
    recipient->deposit(amount);
    save_accounts(accounts);
    
    log_transaction(user->get_account_no(), to_acc, amount, "TRANSFER",
                   "Transfer to " + recipient->get_name());
    
    cout << "\n";
    printSuccess("Transfer successful!");
    printSection("Transfer Details");
    printInfo("From: " + user->get_name() + " (" + to_string(user->get_account_no()) + ")");
    printInfo("To: " + recipient->get_name() + " (" + to_string(to_acc) + ")");
    printInfo("Amount: Rs" + to_string_safe(amount));
    printInfo("Your New Balance: Rs" + to_string_safe(user->get_balance()));
    cout << "\n";
    pauseProgram();
}

// Recurring Payment Function
void setup_recurring(Account* user, vector<Account> &accounts) {
    printHeader("SETUP RECURRING PAYMENT");
    
    cout << "\n";
    int to_acc = 0;
    double amount = 0;
    int interval = 0;
    
    printInput("Recipient Account Number: ");
    string to_acc_str;
    getline(cin, to_acc_str);
    
    if (!is_valid_int(to_acc_str, to_acc)) {
        printError("Invalid account number!");
        pauseProgram();
        return;
    }
    
    Account* recipient = find_account(accounts, to_acc);
    if (!recipient) {
        printError("Account not found!");
        pauseProgram();
        return;
    }
    
    printInput("Amount (Rs): ");
    string amount_str;
    getline(cin, amount_str);
    
    if (!is_valid_double(amount_str, amount) || amount <= 0) {
        printError("Invalid amount!");
        pauseProgram();
        return;
    }
    
    printInput("Interval (Days): ");
    string interval_str;
    getline(cin, interval_str);
    
    if (!is_valid_int(interval_str, interval) || interval <= 0) {
        printError("Invalid interval!");
        pauseProgram();
        return;
    }
    
    vector<RecurringPayment> recurring = load_recurring();
    RecurringPayment rp;
    rp.id = get_next_recurring_id(recurring);
    rp.from_account = user->get_account_no();
    rp.to_account = to_acc;
    rp.amount = amount;
    rp.interval_days = interval;
    rp.last_payment = time(0);
    rp.active = true;
    
    recurring.push_back(rp);
    save_recurring(recurring);
    
    cout << "\n";
    printSuccess("Recurring payment setup successful!");
    printSection("Recurring Payment Details");
    printInfo("From: " + user->get_name());
    printInfo("To: " + recipient->get_name());
    printInfo("Amount: Rs" + to_string_safe(amount));
    printInfo("Interval: Every " + to_string(interval) + " days");
    printInfo("Status: ACTIVE");
    cout << "\n";
    pauseProgram();
}

// View Loans Function
void view_loans(Account* user) {
    printHeader("MY LOANS");
    
    vector<Loan> loans = load_loans();
    bool found = false;
    
    cout << "\n";
    for (auto &l : loans) {
        if (l.account_no == user->get_account_no()) {
            found = true;
            printSection(l.type + " Loan");
            printInfo("Principal: Rs" + to_string_safe(l.principal));
            printInfo("Interest Rate: " + to_string_safe(l.interest_rate) + "%");
            printInfo("Monthly EMI: Rs" + to_string_safe(l.monthly_emi));
            printInfo("Total Duration: " + to_string(l.duration_months) + " months");
            printInfo("Remaining: " + to_string(l.remaining_months) + " months");
            printInfo("Status: " + (l.active ? string("ACTIVE") : string("CLOSED")));
            cout << "\n";
        }
    }
    
    if (!found) {
        printInfo("No active loans found.");
        cout << "\n";
    }
    
    pauseProgram();
}

// View Recurring Payments Function
void view_recurring(Account* user) {
    printHeader("MY RECURRING PAYMENTS");
    
    vector<RecurringPayment> recurring = load_recurring();
    bool found = false;
    
    cout << "\n";
    cout << "  " << string(70, '-') << "\n";
    cout << "  ID  | To Account | Amount (Rs) | Interval (days) | Status\n";
    cout << "  " << string(70, '-') << "\n";
    
    for (auto &r : recurring) {
        if (r.from_account == user->get_account_no()) {
            found = true;
            cout << "  " << setw(3) << r.id << " | " << setw(10) << r.to_account 
                 << " | " << setw(11) << fixed << setprecision(2) << r.amount 
                 << " | " << setw(15) << r.interval_days 
                 << " | " << (r.active ? "ACTIVE" : "INACTIVE") << "\n";
        }
    }
    
    cout << "  " << string(70, '-') << "\n";
    
    if (!found) {
        cout << "\n";
        printInfo("No recurring payments found.");
        cout << "\n";
    }
    
    pauseProgram();
}

// Account Creation
void create_account(vector<Account> &accounts) {
    printHeader("CREATE NEW ACCOUNT");

    string name, pass;
    double bal = 0;

    int acc = generate_unique_account_number(accounts);
    
    cout << "\n";
    printInput("Full Name: ");
    getline(cin, name);

    printInput("Password: ");
    getline(cin, pass);

    printInput("Initial Deposit (Rs): ");
    string bal_str;
    getline(cin, bal_str);
    
    if (!is_valid_double(bal_str, bal)) {
        printError("Invalid amount!");
        pauseProgram();
        return;
    }

    if (bal < 0) {
        printError("Amount cannot be negative!");
        pauseProgram();
        return;
    }

    accounts.push_back(Account(acc, name, pass, bal));
    save_accounts(accounts);

    cout << "\n";
    printSuccess("Account created successfully!");
    printSection("Your Account Details");
    printInfo("Account Number: " + to_string(acc));
    printInfo("Account Name: " + name);
    printInfo("Password: " + pass);
    printInfo("Initial Balance: Rs" + to_string_safe(bal));
    cout << "\n";
    pauseProgram();
}

// Login Function
Account* login(vector<Account> &accounts) {
    printHeader("USER LOGIN");

    int acc = 0;
    string acc_str, pass;

    cout << "\n";
    printInput("Account Number: ");
    getline(cin, acc_str);
    
    if (!is_valid_int(acc_str, acc)) {
        printError("Invalid account number!");
        pauseProgram();
        return nullptr;
    }

    printInput("Password: ");
    getline(cin, pass);

    Account* user = find_account(accounts, acc);
    if (!user) {
        printError("Account not found!");
        pauseProgram();
        return nullptr;
    }

    if (!user->is_active()) {
        printError("Account has been terminated!");
        pauseProgram();
        return nullptr;
    }

    if (user->get_password_hash() != hash_password(pass)) {
        printError("Wrong password!");
        pauseProgram();
        return nullptr;
    }

    cout << "\n";
    printSuccess("Login successful!");
    cout << "\n";
    pauseProgram();
    return user;
}

// User Menu
void user_menu(Account* user, vector<Account> &accounts) {
    int choice = 0;

    do {
        Account* fresh_user = find_account(accounts, user->get_account_no());
        if (!fresh_user || !fresh_user->is_active()) {
            printHeader("ERROR");
            printError("Your account has been terminated!");
            pauseProgram();
            break;
        }
        user = fresh_user;

        printHeader("WELCOME, " + user->get_name());

        cout << "\n  CURRENT BALANCE: Rs" << fixed << setprecision(2) << user->get_balance() << "\n\n";

        printMenuItem(1, "Check Balance");
        printMenuItem(2, "Deposit Money");
        printMenuItem(3, "Transfer Money");
        printMenuItem(4, "Get a Loan");
        printMenuItem(5, "View My Loans");
        printMenuItem(6, "Setup Recurring Payment");
        printMenuItem(7, "View Recurring Payments");
        printMenuItem(8, "EMI Calculator");
        printMenuItem(9, "Credit Card Validator");
        printMenuItem(10, "Logout");

        cout << "\n";
        printInput("Select option: ");
        string choice_str;
        getline(cin, choice_str);
        
        if (!is_valid_int(choice_str, choice)) {
            printError("Invalid input!");
            pauseProgram();
            continue;
        }

        if (choice == 1) {
            printHeader("ACCOUNT BALANCE");
            cout << "\n  BALANCE: Rs" << fixed << setprecision(2) << user->get_balance() << "\n\n";
            pauseProgram();
        }
        else if (choice == 2) {
            printHeader("DEPOSIT MONEY");
            cout << "\n";
            printInput("Deposit Amount (Rs): ");
            string amt_str;
            getline(cin, amt_str);
            
            double amt = 0;
            if (!is_valid_double(amt_str, amt)) {
                printError("Invalid amount!");
                pauseProgram();
                continue;
            }
            
            if (amt > 0) {
                user->deposit(amt);
                save_accounts(accounts);
                log_transaction(user->get_account_no(), user->get_account_no(), amt, "DEPOSIT", "Self Deposit");
                cout << "\n";
                printSuccess("Deposit successful!");
                printInfo("New Balance: Rs" + to_string_safe(user->get_balance()));
                cout << "\n";
            } else {
                printError("Amount must be positive!");
            }
            pauseProgram();
        }
        else if (choice == 3) {
            transfer_money(user, accounts);
        }
        else if (choice == 4) {
            get_loan(user, accounts);
        }
        else if (choice == 5) {
            view_loans(user);
        }
        else if (choice == 6) {
            setup_recurring(user, accounts);
        }
        else if (choice == 7) {
            view_recurring(user);
        }
        else if (choice == 8) {
            calculate_emi(user);
        }
        else if (choice == 9) {
            validate_credit_card(user);
        }

    } while (choice != 10);
}

// Admin Login
bool admin_login() {
    printHeader("ADMIN LOGIN");
    
    string username, password;
    
    cout << "\n";
    printInput("Username: ");
    getline(cin, username);
    
    printInput("Password: ");
    getline(cin, password);
    
    if (username != ADMIN_USERNAME) {
        cout << "\n";
        printError("Invalid admin username!");
        pauseProgram();
        return false;
    }
    
    if (password != ADMIN_PASSWORD) {
        cout << "\n";
        printError("Invalid admin password!");
        pauseProgram();
        return false;
    }
    
    cout << "\n";
    printSuccess("Admin login successful!");
    cout << "\n";
    pauseProgram();
    return true;
}

// Admin Functions
void view_all_users(vector<Account> &accounts) {
    printHeader("ALL REGISTERED USERS");
    
    if (accounts.empty()) {
        cout << "\n";
        printInfo("No users registered yet.");
        cout << "\n";
        pauseProgram();
        return;
    }
    
    cout << "\n";
    cout << "  " << left << setw(12) << "Account #" << setw(20) << "Name" 
         << setw(20) << "Password" << setw(15) << "Balance" << setw(12) << "Status\n";
    cout << "  " << string(70, '-') << "\n";
    
    for (auto &acc : accounts) {
        string status = acc.is_active() ? "ACTIVE" : "TERMINATED";
        cout << "  " << left << setw(12) << acc.get_account_no()
             << setw(20) << acc.get_name()
             << setw(20) << acc.get_plain_password()
             << setw(15) << ("Rs" + to_string_safe(acc.get_balance()))
             << setw(12) << status << "\n";
    }
    cout << "  " << string(70, '-') << "\n";
    cout << "\n";
    printInfo("Total Users: " + to_string(accounts.size()));
    cout << "\n";
    pauseProgram();
}

void view_user_details(vector<Account> &accounts) {
    printHeader("USER DETAILS");
    
    cout << "\n";
    printInput("Account Number: ");
    string acc_str;
    getline(cin, acc_str);
    
    int acc_no = 0;
    if (!is_valid_int(acc_str, acc_no)) {
        cout << "\n";
        printError("Invalid account number!");
        pauseProgram();
        return;
    }
    
    Account* user = find_account(accounts, acc_no);
    if (!user) {
        cout << "\n";
        printError("User not found!");
        pauseProgram();
        return;
    }
    
    cout << "\n";
    printSection("Account Information");
    printInfo("Account Number: " + to_string(user->get_account_no()));
    printInfo("Name: " + user->get_name());
    printInfo("Password: " + user->get_plain_password());
    printInfo("Password Hash: " + user->get_password_hash());
    printInfo("Balance: Rs" + to_string_safe(user->get_balance()));
    printInfo("Status: " + (user->is_active() ? string("ACTIVE") : string("TERMINATED")));
    cout << "\n";
    pauseProgram();
}

void view_all_transactions() {
    printHeader("ALL TRANSACTIONS");
    
    ifstream file("transactions.csv");
    if (!file.is_open()) {
        cout << "\n";
        printInfo("No transactions found.");
        cout << "\n";
        pauseProgram();
        return;
    }
    
    string line;
    cout << "\n";
    cout << "  " << left << setw(12) << "From" << setw(12) << "To" 
         << setw(15) << "Amount" << setw(18) << "Type" << setw(20) << "Description\n";
    cout << "  " << string(77, '-') << "\n";
    
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
        
        if (fields.size() >= 5) {
            cout << "  " << left << setw(12) << fields[0] << setw(12) << fields[1]
                 << setw(15) << fields[2] << setw(18) << fields[3]
                 << setw(20) << (fields.size() > 5 ? fields[5] : "") << "\n";
        }
    }
    
    file.close();
    cout << "  " << string(77, '-') << "\n";
    cout << "\n";
    pauseProgram();
}

void view_all_loans() {
    printHeader("ALL LOANS");
    
    vector<Loan> loans = load_loans();
    if (loans.empty()) {
        cout << "\n";
        printInfo("No loans found.");
        cout << "\n";
        pauseProgram();
        return;
    }
    
    cout << "\n";
    cout << "  " << left << setw(12) << "Account #" << setw(12) << "Type" 
         << setw(15) << "Principal" << setw(12) << "EMI" << setw(15) << "Remaining" 
         << setw(10) << "Status\n";
    cout << "  " << string(76, '-') << "\n";
    
    for (auto &l : loans) {
        cout << "  " << left << setw(12) << l.account_no << setw(12) << l.type
             << setw(15) << ("Rs" + to_string_safe(l.principal))
             << setw(12) << ("Rs" + to_string_safe(l.monthly_emi))
             << setw(15) << (to_string(l.remaining_months) + " months")
             << setw(10) << (l.active ? "ACTIVE" : "CLOSED") << "\n";
    }
    
    cout << "  " << string(76, '-') << "\n";
    cout << "\n";
    pauseProgram();
}

void view_all_recurring() {
    printHeader("ALL RECURRING PAYMENTS");
    
    vector<RecurringPayment> recurring = load_recurring();
    if (recurring.empty()) {
        cout << "\n";
        printInfo("No recurring payments found.");
        cout << "\n";
        pauseProgram();
        return;
    }
    
    cout << "\n";
    cout << "  " << left << setw(6) << "ID" << setw(12) << "From" << setw(12) << "To"
         << setw(15) << "Amount" << setw(12) << "Interval" << setw(10) << "Status\n";
    cout << "  " << string(67, '-') << "\n";
    
    for (auto &r : recurring) {
        cout << "  " << left << setw(6) << r.id << setw(12) << r.from_account 
             << setw(12) << r.to_account << setw(15) << ("Rs" + to_string_safe(r.amount))
             << setw(12) << (to_string(r.interval_days) + " days")
             << setw(10) << (r.active ? "ACTIVE" : "INACTIVE") << "\n";
    }
    
    cout << "  " << string(67, '-') << "\n";
    cout << "\n";
    pauseProgram();
}

void view_system_statistics(vector<Account> &accounts) {
    printHeader("SYSTEM STATISTICS");
    
    int total_accounts = 0;
    int active_accounts = 0;
    int terminated_accounts = 0;
    double total_balance = 0;
    
    vector<Loan> loans = load_loans();
    vector<RecurringPayment> recurring = load_recurring();

    for (auto &acc : accounts) {
        total_accounts++;
        total_balance += acc.get_balance();
        if (acc.is_active()) {
            active_accounts++;
        } else {
            terminated_accounts++;
        }
    }

    cout << "\n";
    printSection("Account Statistics");
    printInfo("Total Accounts: " + to_string(total_accounts));
    printInfo("Active Accounts: " + to_string(active_accounts));
    printInfo("Terminated Accounts: " + to_string(terminated_accounts));
    printInfo("Total System Balance: Rs" + to_string_safe(total_balance));
    
    cout << "\n";
    printSection("Loan Statistics");
    int active_loans = 0;
    double total_loan_principal = 0;
    for (auto &l : loans) {
        if (l.active) {
            active_loans++;
            total_loan_principal += l.principal;
        }
    }
    printInfo("Total Loans: " + to_string(loans.size()));
    printInfo("Active Loans: " + to_string(active_loans));
    printInfo("Total Loan Principal: Rs" + to_string_safe(total_loan_principal));
    
    cout << "\n";
    printSection("Recurring Payment Statistics");
    int active_recurring = 0;
    double total_recurring_amount = 0;
    for (auto &r : recurring) {
        if (r.active) {
            active_recurring++;
            total_recurring_amount += r.amount;
        }
    }
    printInfo("Total Recurring: " + to_string(recurring.size()));
    printInfo("Active Recurring: " + to_string(active_recurring));
    printInfo("Total Monthly Recurring: Rs" + to_string_safe(total_recurring_amount));
    cout << "\n";
    
    pauseProgram();
}

void terminate_account_admin(vector<Account> &accounts) {
    printHeader("TERMINATE ACCOUNT");
    
    cout << "\n";
    printInput("Account Number: ");
    string acc_str;
    getline(cin, acc_str);
    
    int acc_no = 0;
    if (!is_valid_int(acc_str, acc_no)) {
        cout << "\n";
        printError("Invalid account number!");
        pauseProgram();
        return;
    }

    Account* user = find_account(accounts, acc_no);
    if (!user) {
        cout << "\n";
        printError("User not found!");
        pauseProgram();
        return;
    }

    if (!user->is_active()) {
        cout << "\n";
        printError("Account is already terminated!");
        pauseProgram();
        return;
    }

    cout << "\n";
    printWarning("Terminating account: " + user->get_name() + " (" + to_string(acc_no) + ")");
    printWarning("This action cannot be undone!");
    cout << "\n";
    printInput("Type 'yes' to confirm: ");
    string confirm;
    getline(cin, confirm);

    if (confirm == "yes" || confirm == "YES") {
        user->set_active(false);
        save_accounts(accounts);
        log_transaction(acc_no, 0, 0, "ADMIN_TERMINATION", "Account terminated by admin");
        cout << "\n";
        printSuccess("Account terminated successfully!");
        cout << "\n";
    } else {
        cout << "\n";
        printInfo("Termination cancelled.");
        cout << "\n";
    }
    pauseProgram();
}

void admin_menu(vector<Account> &accounts) {
    int choice = 0;
    
    do {
        printHeader("ADMIN DASHBOARD");
        
        cout << "\n";
        printMenuItem(1, "View All Users");
        printMenuItem(2, "View User Details");
        printMenuItem(3, "View System Statistics");
        printMenuItem(4, "View All Transactions");
        printMenuItem(5, "View All Loans");
        printMenuItem(6, "View All Recurring Payments");
        printMenuItem(7, "Terminate Account");
        printMenuItem(8, "Logout");

        cout << "\n";
        printInput("Select option: ");
        string choice_str;
        getline(cin, choice_str);
        
        if (!is_valid_int(choice_str, choice)) {
            printError("Invalid input!");
            pauseProgram();
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
            view_all_transactions();
        }
        else if (choice == 5) {
            view_all_loans();
        }
        else if (choice == 6) {
            view_all_recurring();
        }
        else if (choice == 7) {
            terminate_account_admin(accounts);
        }
        
    } while (choice != 8);
}

// Main Function
int main() {
    vector<Account> accounts = load_accounts();
    int choice = 0;

    do {
        printBanner();
        cout << "\n";
        printMenuItem(1, "Create New Account");
        printMenuItem(2, "User Login");
        printMenuItem(3, "Admin Login");
        printMenuItem(4, "Exit");

        cout << "\n";
        printInput("Select option: ");
        string choice_str;
        getline(cin, choice_str);
        
        if (!is_valid_int(choice_str, choice)) {
            cout << "\n";
            printError("Invalid input!");
            pauseProgram();
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

    printHeader("GOODBYE");
    cout << "\n  Thank you for using Smart Banking System!\n\n";
    return 0;
}