#include <iostream>
#include "banksystem.h"

using namespace std;

int main() {
    cout << "program started\n";

    BankSystem bank;

    int choice;

    do {
        cout << "\n1. create account\n";
        cout << "2. login\n";
        cout << "3. exit\n";

        cin >> choice;

        switch (choice) {
            case 1:
                bank.create_account();
                break;

            case 2: {
                Account* user = bank.login();
                if (user)
                    bank.user_menu(user);
                break;
            }

            case 3:
                cout << "exiting...\n";
                break;

            default:
                cout << "invalid choice\n";
        }

    } while (choice != 3);

    return 0;
}