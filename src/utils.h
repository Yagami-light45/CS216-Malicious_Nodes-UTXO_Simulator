#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
using namespace std;

inline string getTransactionId() {
    static int counter = 0;
    counter++;
    return "tx_" + to_string(counter);
}

inline void printErrorMessage(string s){
    cout << "Error occured: " << s << endl;
}

inline void printGeneralMessage(string s){
    cout << "General Message: " << s << endl;
}

#endif