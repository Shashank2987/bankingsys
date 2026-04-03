#include <mysqlx/xdevapi.h>
#include <iostream>

using namespace std;

int main() {
    try {
        mysqlx::Session session("localhost", 33060, "root", "your_password");

        cout << "Connected successfully!" << endl;

    } catch (const mysqlx::Error &err) {
        cout << "Error: " << err.what() << endl;
    }

    return 0;
}