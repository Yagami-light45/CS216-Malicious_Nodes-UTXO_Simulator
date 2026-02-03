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
    
    // Get sender ID
    cout << "Enter sender ID: ";
    string sender_id;
    cin >> sender_id;
    
    // Show sender's balance before proceeding
    double balance = utxo_manager.get_balance(sender_id);
    cout << "\n" << sender_id << "'s available balance: " << balance << " BTC\n" << endl;
    
    if (balance <= 0) {
        printErrorMessage("No funds available for " + sender_id);
        return false;
    }
    
    // Get recipient ID
    cout << "Enter recipient ID: ";
    string recipient_id;
    cin >> recipient_id;
    
    // Get amount to transfer
    cout << "Enter amount to transfer: ";
    double amount;
    cin >> amount;
    
    if (amount <= 0) {
        printErrorMessage("Amount must be positive");
        return false;
    }
    
    // Get available UTXOs for sender
    vector<UTXO> available_utxos = utxo_manager.getUTXOs(sender_id);
    
    // Sort UTXOs by amount (largest first) for efficient selection
    sort(available_utxos.begin(), available_utxos.end(), 
         [](const UTXO& a, const UTXO& b) {
             return a.getAmountInUTXO() > b.getAmountInUTXO();
         });
    
    // Greedy UTXO selection: select UTXOs until we have enough
    vector<TransactionInputs> inputs;
    double total_input = 0.0;
    
    for (const auto& utxo : available_utxos) {
        if (total_input >= amount) break;  // We have enough
        
        inputs.push_back(TransactionInputs(utxo.getTxId(), utxo.getIndex(), sender_id));
        total_input += utxo.getAmountInUTXO();
    }
    
    // Check if we have enough funds
    if (total_input < amount) {
        printErrorMessage("Insufficient funds. Available: " + to_string(total_input) + " BTC, Required: " + to_string(amount) + " BTC");
        return false;
    }
    
    // Create outputs
    vector<TransactionOutputs> outputs;
    outputs.push_back(TransactionOutputs(amount, recipient_id));  // Payment to recipient
    
    // Calculate change (input - output = change + fee)
    // We'll use a small fee of 0.001 BTC or whatever remains if less
    double change = total_input - amount;
    if (change > 0.001) {
        outputs.push_back(TransactionOutputs(change - 0.001, sender_id));  // Change back to sender (minus small fee)
        cout << "Transaction fee: 0.001 BTC" << endl;
    } else if (change > 0) {
        cout << "Transaction fee: " << change << " BTC (entire remainder)" << endl;
    }
    
    // Display transaction summary
    cout << "\n--- Transaction Summary ---" << endl;
    cout << "From: " << sender_id << endl;
    cout << "To: " << recipient_id << endl;
    cout << "Amount: " << amount << " BTC" << endl;
    cout << "UTXOs used: " << inputs.size() << endl;
    cout << "Total input: " << total_input << " BTC" << endl;
    cout << "---------------------------\n" << endl;
    
    string tx_id = getTransactionId();
    Transaction tx(tx_id, inputs, outputs);
    auto result = mempool.addTransaction(tx, utxo_manager);
    if (!result.first) {
        printErrorMessage(result.second);
        return false;
    }
    printGeneralMessage(result.second);
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
    cout << " 6. Print Block chain" << endl;
    cout << " 7. Exit simulation" << endl;
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

bool mineBlock(UTXO_manager& utxo_manager, Mempool& mempool, BlockChain& blockchain){
    printGeneralMessage("Mining block...");
    mempool.printMempoolCount();
    
    cout<<"Enter miner address: ";
    string minerAdress;
    cin>>minerAdress;
    
    cout<<"How many transactions to include: ";
    int count;
    cin>>count;
    
    // Get top N transactions
    vector<Transaction> included_transactions = mempool.getTopNTransactions(utxo_manager, count);
    if(included_transactions.empty()){
        cout<<"No transactions available to mine.\n";
        return false;
    }

    double totalFees = 0.0;
    static int counter=0;
    // Process transactions
    for(auto& it : included_transactions){
        double fee = calculateFee(it, utxo_manager);
        totalFees += fee;
        removeInputsFromUTXOSet(it, utxo_manager);
        addOutputsToUTXOSet(it, utxo_manager);
        mempool.removeTransaction(it.getTx_id());
    }

    if (totalFees > 0) {
        string coinbaseTxId = "coinbase_" + to_string(counter); 
        utxo_manager.add_utxo(coinbaseTxId, minerAdress, totalFees, -1);
        cout << "Miner " << minerAdress << " rewarded " << totalFees << " BTC (Coinbase).\n";
    }
    
    cout<<"Mining done. " << included_transactions.size() << " transactions confirmed.\n";
    Block newBlock;
    newBlock.setBlockId("block_"+to_string(counter));
    newBlock.setMinerId(minerAdress);
    newBlock.setTransactions(included_transactions);
    blockchain.addBlock(newBlock);
    counter++;  
    return true;
}


void runAllTests();

void runTestScenarios() {
    printGeneralMessage("Running test cases");
    runAllTests();
}

void exitSimulation() {
    printGeneralMessage("Simulation Process complete");
}


void Block::setBlockId(string s){
    this->block_id=s;
}
void Block::setTransactions(vector<Transaction>& transactions){
    this->transactions = transactions;
}
void Block::setMinerId(string& s){
    this->miner_id=s;
}

void BlockChain::addBlock(Block& block){
    this->blockchain.push_back(block);
}
void BlockChain::printBlockchain(){
    if(blockchain.empty()){
        cout<<"Blockchain is empty"<<endl;
        return;
    }
    cout<<"Printing blockchain"<<endl;
    for(auto& it : blockchain){
        it.printBlock();
    }
}


void Block::printBlock(){
    cout<<"----------------------------"<<endl;
    cout<<"Block ID: "<<block_id<<endl;
    cout<<"Miner ID: "<<miner_id<<endl;
    for(auto& it : transactions){
        it.printData();
    }
}