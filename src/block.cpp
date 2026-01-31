#include "block.h"

void removeInputsFromUTXOSet(Transaction& tx, UTXO_manager& utxo_manager) {
    for (auto& it : tx.getTransactionInputs()) {
        utxo_manager.remove_utxo(it.getTx_id(), it.getIndex());
    }
}

void addOutputsToUTXOSet(Transaction& tx, UTXO_manager& utxo_manager) {
    int idx = 0;
    for (auto& it : tx.getTransactionOutputs()) {
        utxo_manager.add_utxo(tx.getTx_id(), it.getAddress(), it.getAmount(), idx);
        idx++;
    }
}

void addMinerUTXO(Transaction& tx, UTXO_manager& utxo_manager, string minerAddress) {
    pair<double, double> fees = getInputSumAndOutputSum(tx, utxo_manager);
    if (fees.first > fees.second) {
        // -1 is the special index for the miner.
        utxo_manager.add_utxo(tx.getTx_id(), minerAddress, fees.first - fees.second, -1);
    }
}

bool createTransaction(Mempool& mempool, UTXO_manager& utxo_manager) {
    printGeneralMessage("Creating new Transaction");
    cout << "Enter user Id: ";
    string user_id;
    cin >> user_id;

    // Show available UTXOs for this user
    vector<UTXO> available_utxos = utxo_manager.getUTXOs(user_id);
    for (auto& it : available_utxos) {
        it.printData();
    }

    // Get transaction inputs manually
    cout << "Enter transaction input count: ";
    int input_count;
    cin >> input_count;
    if (input_count <= 0) {
        printErrorMessage("Invalid input count");
        return false;
    }

    vector<TransactionInputs> inputs(input_count);
    for (int i = 0; i < (int)inputs.size(); i++) {
        string tx_id;
        int index;
        cout << "Enter input transaction ID: ";
        cin >> tx_id;
        cout << "Enter input transaction index: ";
        cin >> index;
        inputs[i].setIndex(index);
        inputs[i].setTx_id(tx_id);
        inputs[i].setOwnerId(user_id);
    }

    // Get transaction outputs manually
    cout << "Enter transaction output count: ";
    int output_count;
    cin >> output_count;
    if (output_count <= 0) {
        printErrorMessage("Invalid output count");
        return false;
    }

    vector<TransactionOutputs> outputs(output_count);
    for (int i = 0; i < (int)outputs.size(); i++) {
        cout << "Enter output amount: ";
        double amount;
        cin >> amount;
        cout << "Enter reciever ID: ";
        string address;
        cin >> address;
        outputs[i].setAmount(amount);
        outputs[i].setOwnerId(address);
    }

    string tx_id = getTransactionId();
    Transaction tx(tx_id, inputs, outputs);
    auto [ok, msg] = mempool.addTransaction(tx, utxo_manager);
    if (!ok) {
        printErrorMessage(msg);
        return false;
    }
    printGeneralMessage(msg);
    mempool.printMempoolCount();
    return true;
}

void createIntialTransactions(UTXO_manager& utxo_manager) {
    cout << "Making Initial Transactions" << endl;
    string genesis_tx = "GENESIS";

    utxo_manager.add_utxo(genesis_tx, "Alice", 50.0, 0);
    utxo_manager.add_utxo(genesis_tx, "Bob", 30.0, 1);
    utxo_manager.add_utxo(genesis_tx, "Charlie", 20.0, 2);
    utxo_manager.add_utxo(genesis_tx, "David", 10.0, 3);
    utxo_manager.add_utxo(genesis_tx, "Eve", 5.0, 4);
}

void printGenesisBlock() {
    cout << "Initial UTXOs (Genesis Block)" << endl;
    cout << "- Alice : 50.0 BTC" << endl;
    cout << "- Bob : 30.0 BTC" << endl;
    cout << "- Charlie : 20.0 BTC" << endl;
    cout << "- David : 10.0 BTC" << endl;
    cout << "- Eve : 5.0 BTC" << endl;
}

void printMainMenu() {
    cout << "Main Menu:" << endl;
    cout << " 1. Create new transaction" << endl;
    cout << " 2. View UTXO set" << endl;
    cout << " 3. View mempool" << endl;
    cout << " 4. Mine block" << endl;
    cout << " 5. Run test scenarios" << endl;
    cout << " 6. exitSimulation" << endl;
}

void viewUTXOset(UTXO_manager& utxo_manager) {
    for (auto& it : utxo_manager.getAllUTXOs()) {
        it.printData();
    }
}

void viewMemPool(Mempool& mempool) {
    printGeneralMessage("Viewing mem pool");
    if (mempool.getTransactions().size() == 0) {
        cout << "Mempool is empty" << endl;
        return;
    }
    for (auto& it : mempool.getTransactions()) {
        it.printData();
    }
}

bool mineBlock(UTXO_manager& utxo_manager, Mempool& mempool) {
    printGeneralMessage("Mining block");
    mempool.printMempoolCount();
    cout << "Enter miner address: ";
    string minerAddress;
    cin >> minerAddress;
    cout << "How many transactions to include: ";
    int included_transaction_count;
    cin >> included_transaction_count;
    vector<Transaction> included_transactions = mempool.getTopNTransactions(utxo_manager, included_transaction_count);
    if ((int)included_transactions.size() < included_transaction_count) {
        cout << "Transactions not available" << endl;
        return false;
    }
    for (auto& it : included_transactions) {
        double fee = calculateFee(it, utxo_manager);
        removeInputsFromUTXOSet(it, utxo_manager);
        addOutputsToUTXOSet(it, utxo_manager);
        if (fee > 0) {
            utxo_manager.add_utxo(it.getTx_id(), minerAddress, fee, -1);
        }
        mempool.removeTransaction(it.getTx_id());
    }
    cout << "Mining done" << endl;
    return true;
}

// Forward declaration - implemented in tests/test_scenarios.cpp
void runAllTests();

void runTestScenarios() {
    printGeneralMessage("Running test cases");
    runAllTests();
}

void exitSimulation() {
    printGeneralMessage("Simulation Process complete");
}

