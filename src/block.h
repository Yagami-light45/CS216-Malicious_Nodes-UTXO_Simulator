#ifndef BLOCK_H
#define BLOCK_H

#include "mempool.h"

// Block mining and UTXO manipulation functions
void removeInputsFromUTXOSet(Transaction& tx, UTXO_manager& utxo_manager);
void addOutputsToUTXOSet(Transaction& tx, UTXO_manager& utxo_manager);
void addMinerUTXO(Transaction& tx, UTXO_manager& utxo_manager, string minerAddress);
bool mineBlock(UTXO_manager& utxo_manager, Mempool& mempool);

// Transaction creation
bool createTransaction(Mempool& mempool, UTXO_manager& utxo_manager);

// Genesis block and initialization
void createIntialTransactions(UTXO_manager& utxo_manager);
void printGenesisBlock();

// View functions
void viewUTXOset(UTXO_manager& utxo_manager);
void viewMemPool(Mempool& mempool);

// Menu and control flow
void printMainMenu();
void runTestScenarios();
void exitSimulation();

#endif // BLOCK_H

