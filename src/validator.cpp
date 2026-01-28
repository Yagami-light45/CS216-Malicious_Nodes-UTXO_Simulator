#include "validator.h"

bool checkInputValidity(Transaction& tx, UTXO_manager& utxo_manager) {
    for (auto& it : tx.getTransactionInputs()) {
        if (!utxo_manager.is_unspent(it.getTx_id(), it.getIndex()) || 
            !utxo_manager.isBelongsTo(it.getTx_id(), it.getIndex(), it.getOwner_id())) {
            return false;
        }
    }
    return true;
}

bool checkDuplicates(Transaction& tx) {
    map<pair<string, int>, int> mp;
    for (auto& it : tx.getTransactionInputs()) {
        pair<string, int> p = {it.getTx_id(), it.getIndex()};
        if (mp.find(p) != mp.end()) {
            return false;
        }
        mp[p] = 1;
    }
    return true;
}

bool checkOutputsForNegatives(Transaction& tx) {
    for (auto& it : tx.getTransactionOutputs()) {
        if (it.getAmount() <= 0) {
            return false;
        }
    }
    return true;
}

pair<double, double> getInputSumAndOutputSum(Transaction& tx, UTXO_manager& utxo_manager) {
    double inputSum = 0.0;
    double outputSum = 0.0;
    for (auto& it : tx.getTransactionInputs()) {
        string tx_id = it.getTx_id();
        int index = it.getIndex();
        inputSum += utxo_manager.getUTXOValue(tx_id, index);
    }
    for (auto& it : tx.getTransactionOutputs()) {
        outputSum += it.getAmount();
    }
    return {inputSum, outputSum};
}

bool checkMempoolForUsedUTXOs(Transaction& tx, set<pair<string, int>>& spent_utxos) {
    for (auto& it : tx.getTransactionInputs()) {
        pair<string, int> p = {it.getTx_id(), it.getIndex()};
        if (spent_utxos.count(p) != 0) {
            return false;
        }
    }
    return true;
}

double calculateFee(Transaction& tx, UTXO_manager& utxo_manager) {
    auto [input_sum, output_sum] = getInputSumAndOutputSum(tx, utxo_manager);
    return input_sum - output_sum;
}

