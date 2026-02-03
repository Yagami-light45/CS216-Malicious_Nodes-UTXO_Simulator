#include "mempool.h"

// Adds a transaction to the mempool after performing several validation checks.
pair<bool, string> Mempool::addTransaction(Transaction tx, UTXO_manager& utxo_manager) {
    // Checks to be done
    /*
        Inputs must exist in UTXO set
        No double spending in inputs (same UTXO in same transaction twice)
        Sum(inputs)>=Sum(outputs)
        No negative amounts in outputs
        No conflict with mempool(i.e the current UTXO or a part of it must not be in mempool)
    */
    pair<bool, string> res;
    bool inputs_existence = checkInputValidity(tx, utxo_manager);
    if (!inputs_existence) {
        res = {false, "Inputs do not exist"};
        return res;
    }
    bool inputs_duplicates = checkDuplicates(tx);
    if (!inputs_duplicates) {
        res = {false, "Duplicates exist in inputs"};
        return res;
    }
    bool check_negative_outputs = checkOutputsForNegatives(tx);
    if (!check_negative_outputs) {
        res = {false, "Negative outputs exist"};
        return res;
    }
    pair<double, double> check_values = getInputSumAndOutputSum(tx, utxo_manager);
    if (check_values.first < check_values.second) {
        res = {false, "Value constraints failed"};
        return res;
    }
    bool check_mempool = checkMempoolForUsedUTXOs(tx, spent_UTXOs);
    if (!check_mempool) {
        res = {false, "Few (or) all UTXOs are already in mempool, cannt reuse"};
        return res;
    }
    transactions[tx.getTx_id()] = tx;
    for (auto& it : tx.getTransactionInputs()) {
        spent_UTXOs.insert({it.getTx_id(), it.getIndex()});
    }
    res = {true, "Transaction created and added to mempool"};
    if(transactions.size()>capacity){
        this->evictTransaction(utxo_manager);
        res = {true, "Transaction created and added to mempool, but mempool size exceeded, eviction occured"};
    }
    return res;
}
// Evicts the transaction with the lowest fee from the mempool if the capacity is exceeded.
void Mempool::evictTransaction(UTXO_manager& utxo_manager) {
    auto it_remove = transactions.begin();
    for (auto it = transactions.begin(); it != transactions.end(); ++it) {
        if (calculateFee(it->second, utxo_manager) <
            calculateFee(it_remove->second, utxo_manager)) {
            it_remove = it;
        }
    }
    for (auto& inp : it_remove->second.getTransactionInputs()) {
        spent_UTXOs.erase({inp.getTx_id(), inp.getIndex()});
    }
    transactions.erase(it_remove);
}


vector<Transaction> Mempool::getTransactions() {
    vector<Transaction> res;
    for (auto& it : transactions) {
        res.push_back(it.second);
    }
    return res;
}

// Number of transactions currently in the mempool.
void Mempool::printMempoolCount() {
    cout << "Mempool now has " << transactions.size() << " transactions" << endl;
}

// Removes a transaction from the mempool by its ID.
bool Mempool::removeTransaction(string tx_id) {
    auto it = transactions.find(tx_id);
    if (it == transactions.end()) return false;
    for (auto& in : it->second.getTransactionInputs()) {
        spent_UTXOs.erase({in.getTx_id(), in.getIndex()});
    }
    transactions.erase(it);
    return true;
}

vector<Transaction> Mempool::getTopNTransactions(UTXO_manager& utxo_manager, int n) {
    vector<Transaction> all_transactions = getTransactions();
    sort(all_transactions.begin(), all_transactions.end(),
         [&](Transaction& a, Transaction& b) {
             return calculateFee(a, utxo_manager) > calculateFee(b, utxo_manager);
         });
    if ((int)all_transactions.size() > n) {
        all_transactions.resize(n);
    }
    return all_transactions;
}

// Clears all transactions from the mempool.
void Mempool::clearMempool() {
    transactions.clear();
    spent_UTXOs.clear();
    printGeneralMessage("Mempool cleared");
}

