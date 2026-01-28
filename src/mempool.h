#ifndef MEMPOOL_H
#define MEMPOOL_H

#include "validator.h"

class Mempool {
private:
    unordered_map<string, Transaction> transactions;
    set<pair<string, int>> spent_UTXOs;

public:
    pair<bool, string> addTransaction(Transaction tx, UTXO_manager& utxo_manager);
    vector<Transaction> getTransactions();
    void printMempoolCount();
    bool removeTransaction(string tx_id);
    vector<Transaction> getTopNTransactions(UTXO_manager& utxo_manager, int n);
    void clearMempool();
};

#endif // MEMPOOL_H

