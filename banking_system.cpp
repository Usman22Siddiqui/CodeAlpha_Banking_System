#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <limits>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

struct Transaction {
    string type, fromAcc, toAcc, timeStr;
    double amount{};
};

string nowTime() {
    time_t t = time(nullptr);
    tm *lt = localtime(&t);
    ostringstream oss;
    oss << setfill('0')
        << (1900 + lt->tm_year) << "-"
        << setw(2) << (1 + lt->tm_mon) << "-"
        << setw(2) << lt->tm_mday << " "
        << setw(2) << lt->tm_hour << ":"
        << setw(2) << lt->tm_min << ":"
        << setw(2) << lt->tm_sec;
    return oss.str();
}

string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string upperStr(string s) {
    for (char &c : s) c = (char)toupper((unsigned char)c);
    return s;
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string readLineNonEmpty(const string& prompt) {
    while (true) {
        cout << prompt;
        string s;
        getline(cin, s);
        s = trim(s);
        if (!s.empty()) return s;
        cout << "\033[31mEmpty input not allowed.\033[0m\n";
    }
}

double readAmount(const string& prompt) {
    while (true) {
        cout << prompt;
        double x;
        if (cin >> x && x > 0) {
            clearInput();
            return x;
        }
        cout << "\033[31mInvalid amount. Enter a positive number.\033[0m\n";
        clearInput();
    }
}

int readChoice(int lo, int hi, const string& prompt) {
    while (true) {
        cout << prompt;
        int c;
        if (cin >> c && c >= lo && c <= hi) {
            clearInput();
            return c;
        }
        cout << "\033[31mInvalid choice.\033[0m\n";
        clearInput();
    }
}

struct Account {
    string accountNo, accountType;
    double balance = 0.0;
    vector<Transaction> history;

    void addTx(const Transaction& tx) {
        history.push_back(tx);
        if (history.size() > 2000) history.erase(history.begin());
    }
};

struct Customer {
    string customerId, name, phone;
    vector<string> accountNos;
};

struct BankSystem {
    unordered_map<string, Customer> customers;
    unordered_map<string, Account> accounts;
    int customerCounter = 1001;
    int accountCounter = 500001;

    string normalizeId(const string& s) {
        return upperStr(trim(s));
    }

    void createCustomer() {
        cout << "\n\033[36m\033[1m=== Create Customer ===\033[0m\n";
        string name = readLineNonEmpty("Enter Name : ");
        string phone = readLineNonEmpty("Enter Phone: ");

        Customer c;
        c.customerId = "C" + to_string(customerCounter++);
        c.name = name;
        c.phone = phone;

        customers[c.customerId] = c;

        cout << "\033[32mCustomer created!\033[0m ID: \033[33m" << c.customerId << "\033[0m\n";
    }

    void listAllCustomers() {
        cout << "\n\033[36m\033[1m=== All Customers ===\033[0m\n";
        if (customers.empty()) {
            cout << "\033[33mNo customers yet.\033[0m\n";
            return;
        }

        cout << left << setw(10) << "Cust ID"
             << setw(22) << "Name"
             << setw(16) << "Phone"
             << setw(10) << "Accounts" << "\n";
        cout << "-------------------------------------------------------------\n";

        for (auto &p : customers) {
            auto &c = p.second;
            cout << left << setw(10) << c.customerId
                 << setw(22) << c.name.substr(0, 21)
                 << setw(16) << c.phone.substr(0, 15)
                 << setw(10) << c.accountNos.size()
                 << "\n";
        }
    }

    void createAccount() {
        cout << "\n\033[36m\033[1m=== Create Account ===\033[0m\n";

        if (customers.empty()) {
            cout << "\033[33mNo customers found. Create a customer first.\033[0m\n";
            return;
        }

        cout << "\033[35mTip:\033[0m If you don't remember ID, choose option 9 (List All Customers).\n";
        string custId = normalizeId(readLineNonEmpty("Enter Customer ID (e.g., C1001): "));

        auto it = customers.find(custId);
        if (it == customers.end()) {
            cout << "\033[31mCustomer not found. Make sure ID is correct.\033[0m\n";
            return;
        }

        cout << "Select Account Type:\n\033[34m1) Current\n2) Savings\n3) Student\033[0m\n";
        int t = readChoice(1, 3, "\033[33mChoice: \033[0m");
        string type = (t == 1 ? "Current" : (t == 2 ? "Savings" : "Student"));

        Account a;
        a.accountNo = "A" + to_string(accountCounter++);
        a.accountType = type;
        a.balance = 0.0;

        accounts[a.accountNo] = a;
        it->second.accountNos.push_back(a.accountNo);

        cout << "\033[32mAccount created & linked!\033[0m\n";
        cout << "Customer ID : \033[33m" << it->second.customerId << "\033[0m\n";
        cout << "Account No  : \033[33m" << a.accountNo << "\033[0m | Type: " << type << "\n";
    }

    void showCustomerAccounts() {
        cout << "\n\033[36m\033[1m=== Customer Accounts ===\033[0m\n";
        string custId = normalizeId(readLineNonEmpty("Enter Customer ID: "));

        auto it = customers.find(custId);
        if (it == customers.end()) {
            cout << "\033[31mCustomer not found.\033[0m\n";
            return;
        }

        Customer &cust = it->second;

        cout << "\n\033[35mCustomer:\033[0m " << cust.name
             << " | Phone: " << cust.phone
             << " | ID: " << cust.customerId << "\n";

        if (cust.accountNos.empty()) {
            cout << "\033[33mNo accounts linked to this customer.\033[0m\n";
            return;
        }

        cout << "\n" << left << setw(14) << "Account No"
             << setw(12) << "Type"
             << setw(14) << "Balance" << "\n";
        cout << "--------------------------------------\n";

        for (auto &accNo : cust.accountNos) {
            auto ait = accounts.find(accNo);
            if (ait != accounts.end()) {
                auto &acc = ait->second;
                cout << left << setw(14) << acc.accountNo
                     << setw(12) << acc.accountType
                     << setw(14) << fixed << setprecision(2) << acc.balance
                     << "\n";
            }
        }
    }

    void deposit() {
        cout << "\n\033[36m\033[1m=== Deposit ===\033[0m\n";
        string accNo = normalizeId(readLineNonEmpty("Enter Account No (e.g., A500001): "));
        auto it = accounts.find(accNo);
        if (it == accounts.end()) {
            cout << "\033[31mAccount not found.\033[0m\n";
            return;
        }

        double amt = readAmount("Enter Deposit Amount: ");
        it->second.balance += amt;

        Transaction tx{"Deposit", accNo, accNo, nowTime(), amt};
        it->second.addTx(tx);

        cout << "\033[32mDeposit successful!\033[0m New Balance: \033[33m"
             << fixed << setprecision(2) << it->second.balance << "\033[0m\n";
    }

    void withdraw() {
        cout << "\n\033[36m\033[1m=== Withdraw ===\033[0m\n";
        string accNo = normalizeId(readLineNonEmpty("Enter Account No: "));
        auto it = accounts.find(accNo);
        if (it == accounts.end()) {
            cout << "\033[31mAccount not found.\033[0m\n";
            return;
        }

        double amt = readAmount("Enter Withdrawal Amount: ");
        if (amt > it->second.balance) {
            cout << "\033[31mInsufficient balance.\033[0m\n";
            return;
        }

        it->second.balance -= amt;
        Transaction tx{"Withdraw", accNo, accNo, nowTime(), amt};
        it->second.addTx(tx);

        cout << "\033[32mWithdrawal successful!\033[0m New Balance: \033[33m"
             << fixed << setprecision(2) << it->second.balance << "\033[0m\n";
    }

    void transfer() {
        cout << "\n\033[36m\033[1m=== Fund Transfer ===\033[0m\n";
        string fromNo = normalizeId(readLineNonEmpty("From Account No: "));
        string toNo   = normalizeId(readLineNonEmpty("To Account No  : "));

        if (fromNo == toNo) {
            cout << "\033[31mFrom and To accounts cannot be same.\033[0m\n";
            return;
        }

        auto f = accounts.find(fromNo);
        auto t = accounts.find(toNo);

        if (f == accounts.end() || t == accounts.end()) {
            cout << "\033[31mAccount not found (one or both).\033[0m\n";
            return;
        }

        double amt = readAmount("Enter Transfer Amount: ");
        if (amt > f->second.balance) {
            cout << "\033[31mInsufficient balance in sender account.\033[0m\n";
            return;
        }

        f->second.balance -= amt;
        t->second.balance += amt;

        Transaction tx{"Transfer", fromNo, toNo, nowTime(), amt};
        f->second.addTx(tx);
        t->second.addTx(tx);

        cout << "\033[32mTransfer successful!\033[0m\n";
        cout << "Sender New Balance  : \033[33m" << fixed << setprecision(2) << f->second.balance << "\033[0m\n";
        cout << "Receiver New Balance: \033[33m" << fixed << setprecision(2) << t->second.balance << "\033[0m\n";
    }

    void showAccountInfo() {
        cout << "\n\033[36m\033[1m=== Account Info ===\033[0m\n";
        string accNo = normalizeId(readLineNonEmpty("Enter Account No: "));
        auto it = accounts.find(accNo);
        if (it == accounts.end()) {
            cout << "\033[31mAccount not found.\033[0m\n";
            return;
        }

        auto &acc = it->second;
        cout << "\n\033[35mAccount No:\033[0m " << acc.accountNo << "\n";
        cout << "\033[35mType      :\033[0m " << acc.accountType << "\n";
        cout << "\033[35mBalance   :\033[0m \033[33m" << fixed << setprecision(2) << acc.balance << "\033[0m\n";
        cout << "\033[35mTx Count  :\033[0m " << acc.history.size() << "\n";
    }

    void showTransactions() {
        cout << "\n\033[36m\033[1m=== Transaction History ===\033[0m\n";
        string accNo = normalizeId(readLineNonEmpty("Enter Account No: "));
        auto it = accounts.find(accNo);
        if (it == accounts.end()) {
            cout << "\033[31mAccount not found.\033[0m\n";
            return;
        }

        auto &acc = it->second;
        if (acc.history.empty()) {
            cout << "\033[33mNo transactions found.\033[0m\n";
            return;
        }

        int n = readChoice(1, 10, "Show how many recent transactions? (1-10): ");

        cout << "\n" << left
             << setw(20) << "Time"
             << setw(12) << "Type"
             << setw(14) << "From"
             << setw(14) << "To"
             << setw(12) << "Amount" << "\n";
        cout << "-----------------------------------------------------------------\n";

        int start = (int)acc.history.size() - n;
        if (start < 0) start = 0;

        for (int i = (int)acc.history.size() - 1; i >= start; i--) {
            const auto &tx = acc.history[i];
            cout << left
                 << setw(20) << tx.timeStr
                 << setw(12) << tx.type
                 << setw(14) << tx.fromAcc
                 << setw(14) << tx.toAcc
                 << setw(12) << fixed << setprecision(2) << tx.amount
                 << "\n";
        }
    }

    void menu() {
        cout << "\n\033[36m\033[1m================ BANKING SYSTEM ================\033[0m\n";
        cout << "\033[34m1) Create Customer\n"
             << "2) Create Account\n"
             << "3) Deposit\n"
             << "4) Withdraw\n"
             << "5) Transfer Funds\n"
             << "6) View Account Info\n"
             << "7) View Recent Transactions\n"
             << "8) View Customer Accounts\n"
             << "9) List All Customers\n"
             << "10) Exit\033[0m\n";
    }
};

int main() {
    BankSystem bank;
    char again = 'y';

    while (true) {
        bank.menu();
        int choice = readChoice(1, 10, "\033[33mChoose option (1-10): \033[0m");

        if (choice == 10) {
            cout << "\n\033[32mGoodbye!\033[0m\n";
            break;
        }

        switch (choice) {
            case 1: bank.createCustomer(); break;
            case 2: bank.createAccount(); break;
            case 3: bank.deposit(); break;
            case 4: bank.withdraw(); break;
            case 5: bank.transfer(); break;
            case 6: bank.showAccountInfo(); break;
            case 7: bank.showTransactions(); break;
            case 8: bank.showCustomerAccounts(); break;
            case 9: bank.listAllCustomers(); break;
        }

        while (true) {
            cout << "\n\033[36mUse again? (y/n): \033[0m";
            cin >> again;
            again = (char)tolower((unsigned char)again);
            clearInput();
            if (again == 'y' || again == 'n') break;
            cout << "\033[31mPlease enter only y or n.\033[0m\n";
        }

        if (again == 'n') {
            cout << "\n\033[32mThanks for using Banking System!\033[0m\n";
            break;
        }
    }

    return 0;
}
