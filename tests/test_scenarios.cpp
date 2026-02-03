#include "../src/block.h"
#include <cassert>

// ============================================================================
// Test Helper Functions
// ============================================================================

void printTestHeader(const string& testName) {
    cout << "\n" << string(60, '=') << endl;
    cout << "TEST: " << testName << endl;
    cout << string(60, '=') << endl;
}

void printTestResult(const string& testName, bool passed) {
    if (passed) {
        cout << "[PASS] " << testName << endl;
    } else {
        cout << "[FAIL] " << testName << endl;
    }
}

// Reset UTXO manager to genesis state
void resetToGenesis(UTXO_manager& utxo_manager) {
    // Clear existing UTXOs by creating a fresh manager
    utxo_manager = UTXO_manager();
    
    string genesis_tx = "GENESIS";
    utxo_manager.add_utxo(genesis_tx, "Alice", 50.0, 0);
    utxo_manager.add_utxo(genesis_tx, "Bob", 30.0, 1);
    utxo_manager.add_utxo(genesis_tx, "Charlie", 20.0, 2);
    utxo_manager.add_utxo(genesis_tx, "David", 10.0, 3);
    utxo_manager.add_utxo(genesis_tx, "Eve", 5.0, 4);
}

// ============================================================================
// TEST 1: Basic Valid Transaction
// Alice sends 10 BTC to Bob with change output and fee
// ============================================================================
bool test1_BasicValidTransaction(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 1: Basic Valid Transaction");
    
    cout << "Scenario: Alice sends 10 BTC to Bob" << endl;
    cout << "- Alice has 50 BTC (GENESIS, index 0)" << endl;
    cout << "- Send 10 BTC to Bob" << endl;
    cout << "- Fee: 0.001 BTC" << endl;
    cout << "- Change back to Alice: 39.999 BTC" << endl;
    
    // Create transaction: Alice -> Bob (10 BTC)
    vector<TransactionInputs> inputs;
    inputs.push_back(TransactionInputs("GENESIS", 0, "Alice"));  // 50 BTC
    
    vector<TransactionOutputs> outputs;
    outputs.push_back(TransactionOutputs(10.0, "Bob"));          // To Bob
    outputs.push_back(TransactionOutputs(39.999, "Alice"));      // Change to Alice (fee = 0.001)
    
    Transaction tx("tx_test1", inputs, outputs);
    
    pair<bool, string> result = mempool.addTransaction(tx, utxo_manager);
    bool success = result.first;
    string msg = result.second;
    
    cout << "Result: " << msg << endl;
    
    bool passed = success;
    printTestResult("Basic Valid Transaction", passed);
    
    if (passed) {
        cout << "  - Transaction added to mempool successfully" << endl;
        cout << "  - Fee calculated: 0.001 BTC" << endl;
    }
    
    return passed;
}

// ============================================================================
// TEST 2: Multiple Inputs
// Alice spends two UTXOs (50 + 20 BTC) to send 60 BTC to Bob
// ============================================================================
bool test2_MultipleInputs(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 2: Multiple Inputs");
    
    // First, give Alice another UTXO
    utxo_manager.add_utxo("extra_tx", "Alice", 20.0, 0);
    
    cout << "Scenario: Alice spends two UTXOs together" << endl;
    cout << "- Input 1: 50 BTC (GENESIS, index 0)" << endl;
    cout << "- Input 2: 20 BTC (extra_tx, index 0)" << endl;
    cout << "- Total Input: 70 BTC" << endl;
    cout << "- Send 60 BTC to Bob, Fee: 0.5 BTC, Change: 9.5 BTC" << endl;
    
    vector<TransactionInputs> inputs;
    inputs.push_back(TransactionInputs("GENESIS", 0, "Alice"));  // 50 BTC
    inputs.push_back(TransactionInputs("extra_tx", 0, "Alice")); // 20 BTC
    
    vector<TransactionOutputs> outputs;
    outputs.push_back(TransactionOutputs(60.0, "Bob"));          // To Bob
    outputs.push_back(TransactionOutputs(9.5, "Alice"));         // Change (fee = 0.5)
    
    Transaction tx("tx_test2", inputs, outputs);
    
    pair<bool, string> result = mempool.addTransaction(tx, utxo_manager);
    bool success = result.first;
    string msg = result.second;
    
    cout << "Result: " << msg << endl;
    
    bool passed = success;
    printTestResult("Multiple Inputs", passed);
    
    return passed;
}

// ============================================================================
// TEST 3: Double-Spend in Same Transaction
// Transaction tries to spend same UTXO twice
// Expected: REJECT
// ============================================================================
bool test3_DoubleSpendSameTransaction(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 3: Double-Spend in Same Transaction");
    
    cout << "Scenario: Transaction tries to spend same UTXO twice" << endl;
    cout << "- Input 1: Alice's 50 BTC (GENESIS, 0)" << endl;
    cout << "- Input 2: Alice's 50 BTC (GENESIS, 0) <- DUPLICATE!" << endl;
    cout << "Expected: REJECT with 'Duplicates exist in inputs'" << endl;
    
    vector<TransactionInputs> inputs;
    inputs.push_back(TransactionInputs("GENESIS", 0, "Alice"));  // First use
    inputs.push_back(TransactionInputs("GENESIS", 0, "Alice"));  // DUPLICATE - same UTXO!
    
    vector<TransactionOutputs> outputs;
    outputs.push_back(TransactionOutputs(90.0, "Bob"));  // Trying to double-spend
    
    Transaction tx("tx_test3", inputs, outputs);
    
    pair<bool, string> result = mempool.addTransaction(tx, utxo_manager);
    bool success = result.first;
    string msg = result.second;
    
    cout << "Result: " << msg << endl;
    
    bool passed = !success;  // Should be rejected
    printTestResult("Double-Spend in Same TX Detection", passed);
    
    if (passed) {
        cout << "  - Correctly rejected duplicate inputs!" << endl;
    }
    
    return passed;
}

// ============================================================================
// TEST 4: Mempool Double-Spend
// TX1: Alice -> Bob (spends UTXO)
// TX2: Alice -> Charlie (spends SAME UTXO)
// Expected: TX1 accepted, TX2 rejected
// ============================================================================
bool test4_MempoolDoubleSpend(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 4: Mempool Double-Spend");
    
    cout << "Scenario: Two transactions try to spend the same UTXO" << endl;
    cout << "- TX1: Alice sends 40 BTC to Bob (using GENESIS,0)" << endl;
    cout << "- TX2: Alice sends 40 BTC to Charlie (using SAME GENESIS,0)" << endl;
    cout << "Expected: TX1 accepted, TX2 rejected" << endl;
    
    // TX1: Alice -> Bob
    vector<TransactionInputs> inputs1;
    inputs1.push_back(TransactionInputs("GENESIS", 0, "Alice"));
    
    vector<TransactionOutputs> outputs1;
    outputs1.push_back(TransactionOutputs(40.0, "Bob"));
    outputs1.push_back(TransactionOutputs(9.9, "Alice"));  // Change
    
    Transaction tx1("tx_test4_1", inputs1, outputs1);
    
    pair<bool, string> result1 = mempool.addTransaction(tx1, utxo_manager);
    bool success1 = result1.first;
    string msg1 = result1.second;
    cout << "TX1 Result: " << msg1 << endl;
    
    // TX2: Alice -> Charlie (same UTXO!)
    vector<TransactionInputs> inputs2;
    inputs2.push_back(TransactionInputs("GENESIS", 0, "Alice"));  // SAME UTXO!
    
    vector<TransactionOutputs> outputs2;
    outputs2.push_back(TransactionOutputs(40.0, "Charlie"));
    outputs2.push_back(TransactionOutputs(9.9, "Alice"));
    
    Transaction tx2("tx_test4_2", inputs2, outputs2);
    
    auto result2 = mempool.addTransaction(tx2, utxo_manager);
    bool success2 = result2.first;
    string msg2 = result2.second;
    cout << "TX2 Result: " << msg2 << endl;
    
    bool passed = success1 && !success2;  // TX1 accepted, TX2 rejected
    printTestResult("Mempool Double-Spend Prevention", passed);
    
    if (passed) {
        cout << "  - TX1 correctly accepted (first-seen rule)" << endl;
        cout << "  - TX2 correctly rejected (UTXO already in mempool)" << endl;
    }
    
    return passed;
}

// ============================================================================
// TEST 5: Insufficient Funds
// Bob tries to send 35 BTC (has only 30 BTC)
// Expected: REJECT
// ============================================================================
bool test5_InsufficientFunds(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 5: Insufficient Funds");
    
    cout << "Scenario: Bob tries to send more than he has" << endl;
    cout << "- Bob has: 30 BTC (GENESIS, index 1)" << endl;
    cout << "- Bob tries to send: 35 BTC to Charlie" << endl;
    cout << "Expected: REJECT with 'Value constraints failed'" << endl;
    
    vector<TransactionInputs> inputs;
    inputs.push_back(TransactionInputs("GENESIS", 1, "Bob"));  // 30 BTC
    
    vector<TransactionOutputs> outputs;
    outputs.push_back(TransactionOutputs(35.0, "Charlie"));  // Trying to send 35 BTC!
    
    Transaction tx("tx_test5", inputs, outputs);
    
    auto result = mempool.addTransaction(tx, utxo_manager);
    bool success = result.first;
    string msg = result.second;
    
    cout << "Result: " << msg << endl;
    
    bool passed = !success;  // Should be rejected
    printTestResult("Insufficient Funds Detection", passed);
    
    return passed;
}

// ============================================================================
// TEST 6: Negative Amount
// Transaction with negative output amount
// Expected: REJECT immediately
// ============================================================================
bool test6_NegativeAmount(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 6: Negative Amount");
    
    cout << "Scenario: Transaction with negative output amount" << endl;
    cout << "- Input: Alice's 50 BTC" << endl;
    cout << "- Output: -10 BTC to Bob (INVALID!)" << endl;
    cout << "Expected: REJECT with 'Negative outputs exist'" << endl;
    
    vector<TransactionInputs> inputs;
    inputs.push_back(TransactionInputs("GENESIS", 0, "Alice"));
    
    vector<TransactionOutputs> outputs;
    outputs.push_back(TransactionOutputs(-10.0, "Bob"));  // NEGATIVE AMOUNT!
    outputs.push_back(TransactionOutputs(60.0, "Alice")); // Even with "extra" change
    
    Transaction tx("tx_test6", inputs, outputs);
    
    auto result = mempool.addTransaction(tx, utxo_manager);
    bool success = result.first;
    string msg = result.second;
    
    cout << "Result: " << msg << endl;
    
    bool passed = !success;  // Should be rejected
    printTestResult("Negative Amount Detection", passed);
    
    return passed;
}

// ============================================================================
// TEST 7: Zero Fee Transaction
// Inputs = Outputs (fee = 0)
// Expected: ACCEPTED (valid in Bitcoin)
// ============================================================================
bool test7_ZeroFeeTransaction(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 7: Zero Fee Transaction");
    
    cout << "Scenario: Transaction with zero fee" << endl;
    cout << "- Input: Bob's 30 BTC (GENESIS, index 1)" << endl;
    cout << "- Output: 30 BTC to Charlie (fee = 0)" << endl;
    cout << "Expected: ACCEPTED (zero fee is valid)" << endl;
    
    vector<TransactionInputs> inputs;
    inputs.push_back(TransactionInputs("GENESIS", 1, "Bob"));  // 30 BTC
    
    vector<TransactionOutputs> outputs;
    outputs.push_back(TransactionOutputs(30.0, "Charlie"));  // Exact amount, no fee
    
    Transaction tx("tx_test7", inputs, outputs);
    
    pair<bool, string> result = mempool.addTransaction(tx, utxo_manager);
    bool success = result.first;
    string msg = result.second;
    
    cout << "Result: " << msg << endl;
    
    double fee = calculateFee(tx, utxo_manager);
    cout << "Calculated Fee: " << fee << " BTC" << endl;
    
    bool passed = success && (fee == 0.0);
    printTestResult("Zero Fee Transaction", passed);
    
    return passed;
}

// ============================================================================
// TEST 8: Race Attack Simulation
// Low-fee merchant TX arrives first, high-fee attack TX arrives second
// Expected: First transaction wins (first-seen rule)
// ============================================================================
bool test8_RaceAttackSimulation(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 8: Race Attack Simulation");
    
    cout << "Scenario: Race attack - attacker tries to replace low-fee TX" << endl;
    cout << "- TX1 (Merchant): Charlie sends 15 BTC to David, low fee (0.001)" << endl;
    cout << "- TX2 (Attack): Charlie sends 15 BTC to Eve, HIGH fee (5.0)" << endl;
    cout << "Expected: TX1 wins (first-seen rule), TX2 rejected" << endl;
    
    // TX1: Low-fee merchant transaction (arrives first)
    vector<TransactionInputs> inputs1;
    inputs1.push_back(TransactionInputs("GENESIS", 2, "Charlie"));  // 20 BTC
    
    vector<TransactionOutputs> outputs1;
    outputs1.push_back(TransactionOutputs(15.0, "David"));    // To merchant
    outputs1.push_back(TransactionOutputs(4.999, "Charlie")); // Change (fee = 0.001)
    
    Transaction tx1("tx_test8_merchant", inputs1, outputs1);
    
    auto result1 = mempool.addTransaction(tx1, utxo_manager);
    bool success1 = result1.first;
    string msg1 = result1.second;
    cout << "TX1 (Merchant, low fee) Result: " << msg1 << endl;
    
    // TX2: High-fee attack transaction (arrives second)
    vector<TransactionInputs> inputs2;
    inputs2.push_back(TransactionInputs("GENESIS", 2, "Charlie"));  // SAME UTXO!
    
    vector<TransactionOutputs> outputs2;
    outputs2.push_back(TransactionOutputs(10.0, "Eve"));      // To attacker
    outputs2.push_back(TransactionOutputs(5.0, "Charlie"));   // Change (fee = 5.0 - HIGH!)
    
    Transaction tx2("tx_test8_attack", inputs2, outputs2);
    
    auto result2 = mempool.addTransaction(tx2, utxo_manager);
    bool success2 = result2.first;
    string msg2 = result2.second;
    cout << "TX2 (Attack, high fee) Result: " << msg2 << endl;
    
    bool passed = success1 && !success2;
    printTestResult("Race Attack Prevention (First-Seen Rule)", passed);
    
    if (passed) {
        cout << "  - First-seen rule enforced!" << endl;
        cout << "  - Merchant transaction protected despite lower fee" << endl;
    }
    
    return passed;
}

// ============================================================================
// TEST 9: Complete Mining Flow
// Add multiple transactions, mine a block, verify UTXOs updated
// ============================================================================
bool test9_CompleteMiningFlow(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 9: Complete Mining Flow");
    
    cout << "Scenario: Full mining cycle" << endl;
    cout << "1. Add transactions to mempool" << endl;
    cout << "2. Mine a block" << endl;
    cout << "3. Verify: UTXOs updated, miner gets fees, mempool cleared" << endl;
    
    // Clear mempool for fresh test
    mempool.clearMempool();
    
    // TX1: Alice -> Bob (10 BTC, fee = 0.1)
    vector<TransactionInputs> inputs1;
    inputs1.push_back(TransactionInputs("GENESIS", 0, "Alice"));  // 50 BTC
    
    vector<TransactionOutputs> outputs1;
    outputs1.push_back(TransactionOutputs(10.0, "Bob"));
    outputs1.push_back(TransactionOutputs(39.9, "Alice"));  // fee = 0.1
    
    Transaction tx1("tx_mine1", inputs1, outputs1);
    mempool.addTransaction(tx1, utxo_manager);
    
    // TX2: Bob -> Charlie (5 BTC, fee = 0.2)
    vector<TransactionInputs> inputs2;
    inputs2.push_back(TransactionInputs("GENESIS", 1, "Bob"));  // 30 BTC
    
    vector<TransactionOutputs> outputs2;
    outputs2.push_back(TransactionOutputs(5.0, "Charlie"));
    outputs2.push_back(TransactionOutputs(24.8, "Bob"));  // fee = 0.2
    
    Transaction tx2("tx_mine2", inputs2, outputs2);
    mempool.addTransaction(tx2, utxo_manager);
    
    cout << "\nBefore Mining:" << endl;
    cout << "- Mempool has " << mempool.getTransactions().size() << " transactions" << endl;
    cout << "- Alice balance: " << utxo_manager.get_balance("Alice") << " BTC" << endl;
    cout << "- Bob balance: " << utxo_manager.get_balance("Bob") << " BTC" << endl;
    
    // Mine the block
    string minerAddress = "Miner";
    vector<Transaction> txsToMine = mempool.getTopNTransactions(utxo_manager, 2);
    
    double totalFees = 0.0;
    for (auto& tx : txsToMine) {
        double fee = calculateFee(tx, utxo_manager);
        totalFees += fee;
        removeInputsFromUTXOSet(tx, utxo_manager);
        addOutputsToUTXOSet(tx, utxo_manager);
        if (fee > 0) {
            utxo_manager.add_utxo(tx.getTx_id(), minerAddress, fee, -1);
        }
        mempool.removeTransaction(tx.getTx_id());
    }
    
    cout << "\nAfter Mining:" << endl;
    cout << "- Mempool has " << mempool.getTransactions().size() << " transactions" << endl;
    cout << "- Alice balance: " << utxo_manager.get_balance("Alice") << " BTC" << endl;
    cout << "- Bob balance: " << utxo_manager.get_balance("Bob") << " BTC" << endl;
    cout << "- Charlie balance: " << utxo_manager.get_balance("Charlie") << " BTC" << endl;
    cout << "- Miner earned: " << utxo_manager.get_balance("Miner") << " BTC in fees" << endl;
    
    // Verify results
    bool mempoolCleared = mempool.getTransactions().size() == 0;
    bool minerGotFees = utxo_manager.get_balance("Miner") > 0;
    bool aliceBalanceUpdated = utxo_manager.get_balance("Alice") < 50.0;
    
    bool passed = mempoolCleared && minerGotFees && aliceBalanceUpdated;
    printTestResult("Complete Mining Flow", passed);
    
    if (passed) {
        cout << "  - Mempool cleared after mining" << endl;
        cout << "  - Miner received transaction fees" << endl;
        cout << "  - UTXO set properly updated" << endl;
    }
    
    return passed;
}

// ============================================================================
// TEST 10: Unconfirmed Chain
// Alice -> Bob creates new UTXO, Bob tries to spend it before mining
// Design decision: We REJECT spending unconfirmed UTXOs
// ============================================================================
bool test10_UnconfirmedChain(UTXO_manager& utxo_manager, Mempool& mempool) {
    printTestHeader("Test 10: Unconfirmed Chain (Spending Unconfirmed UTXO)");
    
    cout << "Scenario: Spending an unconfirmed UTXO" << endl;
    cout << "- TX1: David sends 8 BTC to Frank (creates new UTXO for Frank)" << endl;
    cout << "- TX2: Frank tries to spend the 8 BTC before TX1 is mined" << endl;
    cout << "Design Decision: REJECT (cannot spend unconfirmed UTXO)" << endl;
    
    // TX1: David -> Frank
    vector<TransactionInputs> inputs1;
    inputs1.push_back(TransactionInputs("GENESIS", 3, "David"));  // 10 BTC
    
    vector<TransactionOutputs> outputs1;
    outputs1.push_back(TransactionOutputs(8.0, "Frank"));
    outputs1.push_back(TransactionOutputs(1.9, "David"));  // Change
    
    Transaction tx1("tx_unconf1", inputs1, outputs1);
    
    pair<bool, string> result1 = mempool.addTransaction(tx1, utxo_manager);
    bool success1 = result1.first;
    string msg1 = result1.second;
    cout << "TX1 (David -> Frank) Result: " << msg1 << endl;
    
    // TX2: Frank tries to spend the unconfirmed UTXO
    // The UTXO (tx_unconf1, 0) doesn't exist in UTXO set yet!
    vector<TransactionInputs> inputs2;
    inputs2.push_back(TransactionInputs("tx_unconf1", 0, "Frank"));  // This UTXO doesn't exist yet!
    
    vector<TransactionOutputs> outputs2;
    outputs2.push_back(TransactionOutputs(7.0, "George"));
    
    Transaction tx2("tx_unconf2", inputs2, outputs2);
    
    pair<bool, string> result2 = mempool.addTransaction(tx2, utxo_manager);
    bool success2 = result2.first;
    string msg2 = result2.second;
    cout << "TX2 (Frank spends unconfirmed) Result: " << msg2 << endl;
    
    // Our design: Reject spending unconfirmed UTXOs
    bool passed = success1 && !success2;
    printTestResult("Unconfirmed Chain Prevention", passed);
    
    if (passed) {
        cout << "  - TX1 accepted into mempool" << endl;
        cout << "  - TX2 rejected (UTXO not yet in confirmed set)" << endl;
        cout << "  - Design: Simpler approach - only spend confirmed UTXOs" << endl;
    }
    
    return passed;
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================
void runAllTests() {
    cout << "\n" << string(70, '*') << endl;
    cout << "    BITCOIN UTXO SIMULATOR - TEST SUITE" << endl;
    cout << "    CS216: Introduction to Blockchain" << endl;
    cout << string(70, '*') << endl;
    
    int passed = 0;
    int total = 10;
    
    // Fresh state for each test group
    UTXO_manager utxo_manager;
    Mempool mempool;
    
    // Test 1: Basic Valid Transaction
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test1_BasicValidTransaction(utxo_manager, mempool)) passed++;
    
    // Test 2: Multiple Inputs
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test2_MultipleInputs(utxo_manager, mempool)) passed++;
    
    // Test 3: Double-Spend in Same Transaction
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test3_DoubleSpendSameTransaction(utxo_manager, mempool)) passed++;
    
    // Test 4: Mempool Double-Spend
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test4_MempoolDoubleSpend(utxo_manager, mempool)) passed++;
    
    // Test 5: Insufficient Funds
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test5_InsufficientFunds(utxo_manager, mempool)) passed++;
    
    // Test 6: Negative Amount
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test6_NegativeAmount(utxo_manager, mempool)) passed++;
    
    // Test 7: Zero Fee Transaction
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test7_ZeroFeeTransaction(utxo_manager, mempool)) passed++;
    
    // Test 8: Race Attack Simulation
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test8_RaceAttackSimulation(utxo_manager, mempool)) passed++;
    
    // Test 9: Complete Mining Flow
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test9_CompleteMiningFlow(utxo_manager, mempool)) passed++;
    
    // Test 10: Unconfirmed Chain
    resetToGenesis(utxo_manager);
    mempool.clearMempool();
    if (test10_UnconfirmedChain(utxo_manager, mempool)) passed++;
    
    // Summary
    cout << "\n" << string(70, '=') << endl;
    cout << "TEST SUMMARY" << endl;
    cout << string(70, '=') << endl;
    cout << "Passed: " << passed << "/" << total << " tests" << endl;
    
    if (passed == total) {
        cout << "\n*** ALL TESTS PASSED! ***" << endl;
    } else {
        cout << "\nSome tests failed. Please review the output above." << endl;
    }
    cout << string(70, '=') << endl;
}

// If running test file directly
#ifdef TEST_MAIN
int main() {
    runAllTests();
    return 0;
}
#endif

