#include "block.h"

int main() {
    Mempool mempool;
    UTXO_manager utxo_manager;
    cout << "=== Bitcoin Transaction Simulator ===" << endl;
    createIntialTransactions(utxo_manager);
    printGenesisBlock();
    printMainMenu();
    int userInput;
    cout << "Enter Choice: ";
    while (cin >> userInput) {
        switch (userInput) {
            case 1:
                createTransaction(mempool, utxo_manager);
                cout << "Enter Choice: ";
                break;
            case 2:
                viewUTXOset(utxo_manager);
                cout << "Enter Choice: ";
                break;
            case 3:
                viewMemPool(mempool);
                cout << "Enter Choice: ";
                break;
            case 4:
                mineBlock(utxo_manager, mempool);
                cout << "Enter Choice: ";
                break;
            case 5:
                runTestScenarios();
                cout << "Enter Choice: ";
                break;
            case 6:
                exitSimulation();
                return 0;
            default:
                printErrorMessage("Invalid Choice please choose one from the available options");
                cout << "Enter Choice: ";
        }
    }
    return 0;
}

