#include "block.h"

void printSpaces(){
    cout<<endl;
    cout<<"*****************************************"<<endl;
    cout<<endl;
}

int main() {
    Mempool mempool;
    UTXO_manager utxo_manager;
    BlockChain blockchain;
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
                printSpaces();
                cout << "Enter Choice: ";
                break;
            case 2:
                viewUTXOset(utxo_manager);
                printSpaces();
                cout << "Enter Choice: ";
                break;
            case 3:
                viewMemPool(mempool);
                printSpaces();
                cout << "Enter Choice: ";
                break;
            case 4:
                mineBlock(utxo_manager, mempool, blockchain);
                printSpaces();
                cout << "Enter Choice: ";
                break;
            case 5:
                runTestScenarios();
                printSpaces();
                cout << "Enter Choice: ";
                break;
            case 6:
                blockchain.printBlockchain();
                printSpaces();
                cout<<"Enter choice: ";
                break;
            case 7:
                exitSimulation();
                printSpaces();
                return 0;
            default:
                printErrorMessage("Invalid Choice please choose one from the available options");
                cout << "Enter Choice: ";
        }
    }
    return 0;
}

