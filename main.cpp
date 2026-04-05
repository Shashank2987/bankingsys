#include <iostream>
#include "banksystem.h"

using namespace std;

int main() {
    BankSystem bank;
    int choice;

    do {
        cout << "\n1. create account\n2. login\n3. exit\n";
        cin >> choice;

        if (choice == 1) {
            bank.create_account();
        }
        else if (choice == 2) {
            Account* user = bank.login();
            if (user)
                bank.user_menu(user);
        }

    } while (choice != 3);

    return 0;
}