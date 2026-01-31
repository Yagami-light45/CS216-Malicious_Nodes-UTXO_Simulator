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

    // Check if user has any UTXOs
    vector<UTXO> available_utxos = utxo_manager.getUTXOs(user_id);
    if (available_utxos.empty()) {
        printErrorMessage("No UTXOs available for this user");
        return false;
    }

    // Show user's balance
    double user_balance = utxo_manager.get_balance(user_id);
    cout << "Your available balance: " << user_balance << " BTC" << endl;

    cout << "Enter amount to send: ";
    double amount;
    cin >> amount;

    cout << "Enter recipient address: ";
    string recipient;
    cin >> recipient;

    cout << "Enter transaction fee: ";
    double fee;
    cin >> fee;

    // Validate inputs
    if (amount <= 0) {
        printErrorMessage("Amount must be positive");
        return false;
    }
    if (fee < 0) {
        printErrorMessage("Fee cannot be negative");
        return false;
    }

    double total_needed = amount + fee;

    if (total_needed > user_balance) {
        printErrorMessage("Insufficient balance for this transaction");
        return false;
    }

    // Automatically select UTXOs to cover the total needed
    vector<TransactionInputs> inputs;
    double input_sum = 0.0;

    for (auto& utxo : available_utxos) {
        if (input_sum >= total_needed) {
            break;
        }
        TransactionInputs inp(utxo.getTxId(), utxo.getIndex(), user_id);
        inputs.push_back(inp);
        input_sum += utxo.getAmountInUTXO();
    }

    // Create transaction outputs
    vector<TransactionOutputs> outputs;

    // Output to recipient
    TransactionOutputs recipient_output(amount, recipient);
    outputs.push_back(recipient_output);

    // Calculate change (input_sum - amount - fee)
    double change = input_sum - amount - fee;
    if (change > 0) {
        TransactionOutputs change_output(change, user_id);
        outputs.push_back(change_output);
    }

    string tx_id = getTransactionId();
    Transaction tx(tx_id, inputs, outputs);
    auto [ok, msg] = mempool.addTransaction(tx, utxo_manager);
    if (!ok) {
        printErrorMessage(msg);
        return false;
    }
    printGeneralMessage(msg);
    cout << "Transaction Summary:" << endl;
    cout << "  Sent: " << amount << " BTC to " << recipient << endl;
    cout << "  Fee: " << fee << " BTC" << endl;
    if (change > 0) {
        cout << "  Change returned: " << change << " BTC" << endl;
    }
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

