#include "transaction.h"

int main() {
    cout << "=== Testing Transaction Feature ===" << endl;

    TransactionInputs in1("tx_100", 0, "Alice");
    vector<TransactionInputs> inputs = { in1 };

    TransactionOutputs out1(5.5, "Bob");
    vector<TransactionOutputs> outputs = { out1 };

    Transaction tx("tx_new_001", inputs, outputs);

    cout << "\nExpected Output:" << endl;
    cout << "Transaction ID tx_new_001" << endl;
    cout << "Inputs: \ntx_100 0 Alice" << endl;
    cout << "Outputs: \n5.5 btc Bob" << endl;
    
    cout << "\nActual Output:" << endl;
    tx.printData();

    return 0;
}