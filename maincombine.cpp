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
#include <unistd.h>

using namespace std;

// ============== COLOR CODES ==============
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string BRIGHT_RED = "\033[91m";
const string BRIGHT_GREEN = "\033[92m";
const string BRIGHT_YELLOW = "\033[93m";
const string BRIGHT_BLUE = "\033[94m";
const string BRIGHT_MAGENTA = "\033[95m";
const string BRIGHT_CYAN = "\033[96m";
const string BRIGHT_WHITE = "\033[97m";
const string BRIGHT_BLACK = "\033[90m";
const string BG_BLUE = "\033[44m";

// Constants
const string SALT = "Smar7_B@nk_2024_#Secure!";
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";

// ============== UTILITY FUNCTIONS ==============
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

// ============== CREDIT CARD VALIDATOR ==============
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
    
    // VISA
    if (cardNum[0] == '4' && (cardNum.length() == 13 || cardNum.length() == 16 || cardNum.length() == 19)) {
        return "VISA";
    }
    
    // MASTERCARD
    if (cardNum.length() >= 2) {
        int first_two = stoi(cardNum.substr(0, 2));
        if ((first_two >= 51 && first_two <= 55) && (cardNum.length() == 16)) {
            return "MASTERCARD";
        }
    }
    
    // AMERICAN EXPRESS
    if (cardNum.length() >= 2) {
        int first_two = stoi(cardNum.substr(0, 2));
        if ((first_two == 34 || first_two == 37) && cardNum.length() == 15) {
            return "AMERICAN EXPRESS";
        }
    }
    
    // DISCOVER
    if (cardNum.length() >= 4) {
        int first_four = stoi(cardNum.substr(0, 4));
        if ((first_four == 6011 || first_four == 6512 || first_four == 6556) && (cardNum.length() == 16)) {
            return "DISCOVER";
        }
    }
    
    // RUPAY
    if (cardNum.length() >= 4) {
        int first_four = stoi(cardNum.substr(0, 4));
        if (first_four >= 6070 && first_four <= 6074 && (cardNum.length() == 16)) {
            return "RUPAY";
        }
    }
    
    // DINERS CLUB
    if (cardNum.length() >= 2) {
        int first_two = stoi(cardNum.substr(0, 2));
        if ((first_two == 36 || first_two == 38 || first_two == 39) && (cardNum.length() == 14)) {
            return "DINERS CLUB";
        }
    }
    
    // JCB
    if (cardNum.length() >= 4) {
        int first_four = stoi(cardNum.substr(0, 4));
        if ((first_four >= 3528 && first_four <= 3589) && (cardNum.length() == 16)) {
            return "JCB";
        }
    }
    
    return "UNKNOWN";
}

// ============== UI FUNCTIONS ==============
void printBanner() {
    clearScreen();
    cout << string(80, '=') << "\n";
    cout << "  [===] SMART BANKING SYSTEM [===]\n";
    cout << string(80, '=') << "\n" << RESET;
}

void printHeader(string title) {
    clearScreen();
    cout << BRIGHT_CYAN << BOLD;
    cout << "\n  [" << string(74, '-') << "]\n";
    cout << "  | " << BRIGHT_GREEN << title << BRIGHT_CYAN;
    cout << string(71 - title.length(), ' ') << "|\n";
    cout << "  [" << string(74, '-') << "]\n" << RESET;
}

void printSuccess(string msg) {
    cout << BRIGHT_GREEN << BOLD << "  [+] " << msg << RESET << "\n";
}

void printError(string msg) {
    cout << BRIGHT_RED << BOLD << "  [-] " << msg << RESET << "\n";
}

void printInfo(string msg) {
    cout << BRIGHT_CYAN << "  [i] " << msg << RESET << "\n";
}

void printWarning(string msg) {
    cout << BRIGHT_YELLOW << BOLD << "  [!] " << msg << RESET << "\n";
}

void printSection(string title) {
    cout << BRIGHT_MAGENTA << BOLD << "\n  >>> " << title << RESET << "\n";
    cout << BRIGHT_MAGENTA << "      " << string(title.length(), '-') << RESET << "\n";
}

void printMenuItem(int num, string text) {
    cout << BRIGHT_CYAN << "  [" << BRIGHT_YELLOW << num << BRIGHT_CYAN << "] " 
         << RESET << text << "\n";
}

void printInput(string prompt) {
    cout << BRIGHT_BLUE << BOLD << "  >> " << RESET << prompt;
}

void pauseProgram() {
    cout << BRIGHT_YELLOW << "\n  Press Enter to continue..." << RESET;
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

// ============== EMI CALCULATOR ==============
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
    
    // Calculate EMI using formula: EMI = [P(r)(1+r)^n] / [(1+r)^n-1]
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
    printInfo("Principal Amount: " + BRIGHT_GREEN + "Rs" + to_string_safe(principal) + RESET);
    printInfo("Interest Rate: " + BRIGHT_GREEN + to_string_safe(interest_rate) + "%" + RESET);
    printInfo("Loan Duration: " + BRIGHT_GREEN + to_string(months) + " months" + RESET);
    printInfo("Monthly EMI: " + BRIGHT_YELLOW + BOLD + "Rs" + to_string_safe(emi) + RESET);
    printInfo("Total Interest: " + BRIGHT_RED + "Rs" + to_string_safe(total_interest) + RESET);
    printInfo("Total Amount Payable: " + BRIGHT_GREEN + BOLD + "Rs" + to_string_safe(total_amount) + RESET);
    cout << "\n";
    
    cout << BRIGHT_CYAN << "  EMI BREAKDOWN:\n";
    cout << "  " << string(70, '-') << "\n";
    cout << "  Year | Monthly EMI | Total Paid | Principal | Interest\n";
    cout << "  " << string(70, '-') << RESET << "\n";
    
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
    cout << RESET;
    cout << "\n";
    pauseProgram();
}

// ============== CREDIT CARD VALIDATOR ==============
void validate_credit_card(Account* user) {
    printHeader("CREDIT CARD VALIDATOR");
    
    cout << "\n";
    printInput("Credit Card Number: ");
    string card_num;
    getline(cin, card_num);
    
    // Remove spaces if any
    card_num.erase(remove(card_num.begin(), card_num.end(), ' '), card_num.end());
    
    // Validate length
    if (card_num.length() < 13 || card_num.length() > 19) {
        printError("Invalid card number length!");
        cout << "\n";
        pauseProgram();
        return;
    }
    
    // Validate all digits
    for (char c : card_num) {
        if (!isdigit(c)) {
            printError("Card number must contain only digits!");
            cout << "\n";
            pauseProgram();
            return;
        }
    }
    
    // Validate using Luhn algorithm
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
    printInfo("Card Type: " + BRIGHT_YELLOW + BOLD + card_type + RESET);
    printInfo("Card Length: " + BRIGHT_CYAN + to_string(card_num.length()) + " digits" + RESET);
    printInfo("First 4 Digits: " + BRIGHT_CYAN + card_num.substr(0, 4) + RESET);
    printInfo("Last 4 Digits: " + BRIGHT_CYAN + string(card_num.length() - 4, '*') + 
              card_num.substr(card_num.length() - 4) + RESET);
    
    // Display card type info
    cout << "\n";
    cout << BRIGHT_MAGENTA << BOLD << "  CARD INFORMATION:\n" << RESET;
    
    if (card_type == "VISA") {
        cout << BRIGHT_CYAN << "  - Starts with 4\n  - Length: 13, 16, or 19 digits\n" << RESET;
    } else if (card_type == "MASTERCARD") {
        cout << BRIGHT_CYAN << "  - Starts with 51-55\n  - Length: 16 digits\n" << RESET;
    } else if (card_type == "AMERICAN EXPRESS") {
        cout << BRIGHT_CYAN << "  - Starts with 34 or 37\n  - Length: 15 digits\n" << RESET;
    } else if (card_type == "DISCOVER") {
        cout << BRIGHT_CYAN << "  - Starts with 6011, 6512, or 6556\n  - Length: 16 digits\n" << RESET;
    } else if (card_type == "RUPAY") {
        cout << BRIGHT_CYAN << "  - Starts with 6070-6074\n  - Length: 16 digits\n" << RESET;
    } else if (card_type == "DINERS CLUB") {
        cout << BRIGHT_CYAN << "  - Starts with 36, 38, or 39\n  - Length: 14 digits\n" << RESET;
    } else if (card_type == "JCB") {
        cout << BRIGHT_CYAN << "  - Starts with 3528-3589\n  - Length: 16 digits\n" << RESET;
    } else {
        cout << BRIGHT_RED << "  - Unknown card type\n" << RESET;
    }
    
    cout << "\n";
    pauseProgram();
}

// ============== ACCOUNT CREATION ==============
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
    printInfo("Account Number: " + BRIGHT_CYAN + BOLD + to_string(acc) + RESET);
    printInfo("Account Name: " + BRIGHT_CYAN + BOLD + name + RESET);
    printInfo("Password: " + BRIGHT_CYAN + BOLD + pass + RESET);
    printInfo("Initial Balance: " + BRIGHT_GREEN + BOLD + "Rs" + to_string_safe(bal) + RESET);
    cout << "\n";
    pauseProgram();
}

// ============== LOGIN ==============
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

// ============== USER MENU ==============
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

        cout << "\n";
        cout << BRIGHT_GREEN << BOLD << "  BALANCE: Rs" << to_string_safe(user->get_balance()) << RESET << "\n\n";

        printMenuItem(1, "Check Balance");
        printMenuItem(2, "Deposit Money");
        printMenuItem(3, "EMI Calculator");
        printMenuItem(4, "Credit Card Validator");
        printMenuItem(5, "Logout");

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
            cout << "\n";
            cout << BRIGHT_GREEN << BOLD << "  BALANCE: Rs" << to_string_safe(user->get_balance()) << RESET << "\n\n";
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
                log_transaction(user->get_account_no(), user->get_account_no(), amt, "deposit");
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
            calculate_emi(user);
        }
        else if (choice == 4) {
            validate_credit_card(user);
        }

    } while (choice != 5);
}

// ============== ADMIN LOGIN ==============
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

// ============== ADMIN FUNCTIONS ==============
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
    cout << BRIGHT_CYAN << BOLD << "  ";
    cout << left << setw(12) << "Account #" << setw(20) << "Name" << setw(20) << "Password"
         << setw(15) << "Balance" << setw(12) << "Status\n";
    cout << "  " << string(70, '=') << RESET << "\n";
    
    for (auto &acc : accounts) {
        string status = acc.is_active() ? BRIGHT_GREEN + string("ACTIVE") + RESET : BRIGHT_RED + string("TERMINATED") + RESET;
        cout << BRIGHT_WHITE << "  ";
        cout << left << setw(12) << acc.get_account_no()
             << setw(20) << acc.get_name()
             << setw(20) << acc.get_plain_password()
             << setw(15) << ("Rs" + to_string_safe(acc.get_balance()))
             << status << "\n";
    }
    cout << BRIGHT_CYAN << "  " << string(70, '=') << RESET << "\n";
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
    printInfo("Account Number: " + BRIGHT_CYAN + to_string(user->get_account_no()) + RESET);
    printInfo("Name: " + BRIGHT_CYAN + user->get_name() + RESET);
    printInfo("Password: " + BRIGHT_YELLOW + user->get_plain_password() + RESET);
    printInfo("Password Hash: " + BRIGHT_BLACK + user->get_password_hash() + RESET);
    printInfo("Balance: " + BRIGHT_GREEN + "Rs" + to_string_safe(user->get_balance()) + RESET);
    printInfo("Status: " + (user->is_active() ? BRIGHT_GREEN + string("ACTIVE") : BRIGHT_RED + string("TERMINATED")) + RESET);
    cout << "\n";
    pauseProgram();
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

    cout << "\n";
    printSection("Account Statistics");
    printInfo("Total Accounts: " + BRIGHT_CYAN + to_string(total_accounts) + RESET);
    printInfo("Active Accounts: " + BRIGHT_GREEN + to_string(active_accounts) + RESET);
    printInfo("Terminated Accounts: " + BRIGHT_RED + to_string(terminated_accounts) + RESET);
    printInfo("Total System Balance: " + BRIGHT_GREEN + "Rs" + to_string_safe(total_balance) + RESET);
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
        log_transaction(acc_no, 0, 0, "admin_termination");
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
        printMenuItem(4, "Terminate Account");
        printMenuItem(5, "Logout");

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
            terminate_account_admin(accounts);
        }
        
    } while (choice != 5);
}

// ============== MAIN ==============
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
    cout << "\n";
    cout << BRIGHT_CYAN << "  Thank you for using Smart Banking System!\n\n" << RESET;
    return 0;
}