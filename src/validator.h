#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "transaction.h"
#include "utxo_manager.h"

// Validation functions for transactions
bool checkInputValidity(Transaction& tx, UTXO_manager& utxo_manager);
bool checkDuplicates(Transaction& tx);
bool checkOutputsForNegatives(Transaction& tx);
pair<double, double> getInputSumAndOutputSum(Transaction& tx, UTXO_manager& utxo_manager);
bool checkMempoolForUsedUTXOs(Transaction& tx, set<pair<string, int>>& spent_utxos);
double calculateFee(Transaction& tx, UTXO_manager& utxo_manager);

#endif // VALIDATOR_H

