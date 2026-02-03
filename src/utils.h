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
//unique transaction id => 

// inline string getTransactionId() {
//     static bool seeded = false;
//     if (!seeded) {
//         srand(time(0));
//         seeded = true;
//     }
//     long timestamp = time(0);
//     int random_num = rand() % 9000 + 1000; 
//     return "tx_" + to_string(timestamp) + "_" + to_string(random_num);
// }

inline void printErrorMessage(string s){
    cout << "Error occured: " << s << endl;
}

inline void printGeneralMessage(string s){
    cout << "General Message: " << s << endl;
}

#endif