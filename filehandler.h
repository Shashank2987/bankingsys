#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <vector>
#include "account.h"

using namespace std;

// load all accounts from csv
vector<Account> load_accounts();

// save all accounts to csv
void save_accounts(vector<Account> &accounts);

#endif