# Bitcoin Transaction & UTXO Simulator

**CS-216: Introduction to Blockchain**

## Team: Malicious Nodes

| Name | Roll Number |
|------|-------------|
| S Varshith Reddy | 240001071 |
| Harshith Pasupuleti | 240003034 |
| Trijal Mathuria | 240001073 |
| D Sujith Reddy | 240001028 |

---

## Table of Contents

1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Data Flow](#data-flow)
4. [Data Structures & Design Decisions](#data-structures--design-decisions)
5. [Component Details](#component-details)
6. [Transaction Validation](#transaction-validation)
7. [Building & Running](#building--running)
8. [Usage Guide](#usage-guide)
9. [Test Scenarios](#test-scenarios)

---

## Overview

This simulator implements a simplified version of Bitcoin's **UTXO (Unspent Transaction Output)** model. It demonstrates how Bitcoin handles transactions, validates them, prevents double-spending, and mines blocks.

### Key Concepts Simulated

- **UTXO Model**: Every transaction consumes existing UTXOs and creates new ones
- **Mempool**: Temporary storage for unconfirmed transactions
- **Double-Spend Prevention**: Both within transactions and across the mempool
- **Fee-based Priority**: Higher-fee transactions get priority during mining
- **First-Seen Rule**: Prevents race attacks by accepting the first valid transaction

---

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                         main.cpp                                │
│                    (Entry Point & Menu)                         │
└─────────────────────────────────────────────────────────────────┘
                              │
          ┌───────────────────┼───────────────────┐
          ▼                   ▼                   ▼
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│   block.cpp     │  │   mempool.cpp   │  │ utxo_manager.cpp│
│   block.h       │  │   mempool.h     │  │ utxo_manager.h  │
│                 │  │                 │  │                 │
│ - Block         │  │ - Mempool       │  │ - UTXO          │
│ - BlockChain    │  │ - Transaction   │  │ - UTXO_manager  │
│ - Mining Logic  │  │   Storage       │  │ - Balance Query │
└─────────────────┘  └─────────────────┘  └─────────────────┘
          │                   │                   │
          └───────────────────┼───────────────────┘
                              ▼
                    ┌─────────────────┐
                    │  validator.cpp  │
                    │  validator.h    │
                    │                 │
                    │ - Input Check   │
                    │ - Duplicate Chk │
                    │ - Fee Calc      │
                    └─────────────────┘
                              │
          ┌───────────────────┴───────────────────┐
          ▼                                       ▼
┌─────────────────┐                    ┌─────────────────┐
│ transaction.cpp │                    │    utils.h      │
│ transaction.h   │                    │                 │
│                 │                    │ - TX ID Gen     │
│ - Tx Inputs     │                    │ - Print Helpers │
│ - Tx Outputs    │                    └─────────────────┘
│ - Transaction   │
└─────────────────┘
```

---

## Data Flow

### Transaction Lifecycle

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│   User       │     │   Mempool    │     │   Mining     │     │  Blockchain  │
│   Creates    │────▶│   Validates  │────▶│   Process    │────▶│   Confirmed  │
│   Transaction│     │   & Stores   │     │   Block      │     │   State      │
└──────────────┘     └──────────────┘     └──────────────┘     └──────────────┘
       │                    │                    │                    │
       │                    │                    │                    │
       ▼                    ▼                    ▼                    ▼
   User Input          Validation           UTXO Updates         Permanent
   - Sender ID         - UTXO exists?       - Remove spent       Record
   - Input UTXOs       - Owner match?       - Add new UTXOs
   - Output amounts    - No duplicates?     - Miner reward
   - Recipients        - Sufficient funds?
                       - Not in mempool?
```

### Detailed Flow: Creating a Transaction

1. **User Input Phase**
   ```
   User enters: sender_id → shows available UTXOs for that user
                         → enters input count and UTXO references
                         → enters output count, amounts, and recipients
   ```

2. **Validation Phase** (in order)
   ```
   ┌─────────────────────────────────────────────────────────────┐
   │ 1. checkInputValidity()                                     │
   │    - Each input UTXO exists in UTXO set                     │
   │    - Each input UTXO belongs to the claimed owner           │
   ├─────────────────────────────────────────────────────────────┤
   │ 2. checkDuplicates()                                        │
   │    - No same UTXO used twice in inputs                      │
   ├─────────────────────────────────────────────────────────────┤
   │ 3. checkOutputsForNegatives()                               │
   │    - All output amounts are positive (> 0)                  │
   ├─────────────────────────────────────────────────────────────┤
   │ 4. getInputSumAndOutputSum()                                │
   │    - Sum(inputs) >= Sum(outputs)                            │
   │    - Difference is the transaction fee                      │
   ├─────────────────────────────────────────────────────────────┤
   │ 5. checkMempoolForUsedUTXOs()                               │
   │    - Input UTXOs not already committed in mempool           │
   │    - Prevents double-spending across pending transactions   │
   └─────────────────────────────────────────────────────────────┘
   ```

3. **Mempool Storage Phase**
   - Transaction added to `unordered_map<string, Transaction>`
   - Input UTXOs tracked in `set<pair<string, int>> spent_UTXOs`
   - If capacity exceeded, lowest-fee transaction evicted

### Detailed Flow: Mining a Block

```
┌────────────────────────────────────────────────────────────────────┐
│ 1. User specifies miner address and transaction count             │
├────────────────────────────────────────────────────────────────────┤
│ 2. getTopNTransactions() - Sort by fee, select top N              │
├────────────────────────────────────────────────────────────────────┤
│ 3. For each transaction:                                          │
│    a. Calculate fee (before modifying UTXO set)                   │
│    b. removeInputsFromUTXOSet() - Mark inputs as spent            │
│    c. addOutputsToUTXOSet() - Create new UTXOs                    │
│    d. Remove transaction from mempool                             │
├────────────────────────────────────────────────────────────────────┤
│ 4. Create coinbase UTXO for miner (sum of all fees)               │
├────────────────────────────────────────────────────────────────────┤
│ 5. Create Block and add to Blockchain                             │
└────────────────────────────────────────────────────────────────────┘
```

---

## Data Structures & Design Decisions

### Why `unordered_map` for Mempool?

```cpp
unordered_map<string, Transaction> transactions;
```

| Aspect | Reasoning |
|--------|-----------|
| **O(1) Lookup** | Quick retrieval by transaction ID when mining or removing |
| **O(1) Insert** | Adding new transactions is constant time |
| **O(1) Delete** | Removing mined transactions is efficient |
| **No Ordering Needed** | Transaction order determined by fee during mining, not insertion order |

### Why `map` for UTXO Set?

```cpp
map<pair<string, int>, UTXO> UTXO_set;
```

| Aspect | Reasoning |
|--------|-----------|
| **Composite Key** | UTXOs uniquely identified by (transaction_id, output_index) pair |
| **Sorted Order** | Enables predictable iteration when displaying UTXOs |
| **O(log n) Operations** | Acceptable tradeoff for ordered traversal benefits |
| **Pair Key** | Natural representation - same as Bitcoin's outpoint |

### Why `set` for Spent UTXOs Tracking in Mempool?

```cpp
set<pair<string, int>> spent_UTXOs;
```

| Aspect | Reasoning |
|--------|-----------|
| **Quick Membership Test** | O(log n) check if UTXO already committed |
| **No Duplicates** | Set inherently prevents double-tracking |
| **Easy Cleanup** | Simple erase when transaction removed/mined |

### Why Vectors for Transaction Inputs/Outputs?

```cpp
vector<TransactionInputs> inputs;
vector<TransactionOutputs> outputs;
```

| Aspect | Reasoning |
|--------|-----------|
| **Variable Size** | Transactions can have any number of inputs/outputs |
| **Index-Based Access** | Output index (0, 1, 2...) used for UTXO identification |
| **Contiguous Memory** | Efficient iteration during validation |

---

## Component Details

### Transaction Structure

```
Transaction
├── tx_id: string           // Unique identifier (auto-generated)
├── inputs: vector          // UTXOs being spent
│   └── TransactionInputs
│       ├── tx_id: string   // Reference to source transaction
│       ├── index: int      // Output index in source transaction
│       └── owner_id: string// Claimed owner (for verification)
└── outputs: vector         // New UTXOs being created
    └── TransactionOutputs
        ├── amount: double  // BTC amount
        └── address: string // Recipient identifier
```

### UTXO Structure

```
UTXO
├── tx_id: string     // Transaction that created this UTXO
├── index: int        // Output index within that transaction
├── owner_id: string  // Who can spend this UTXO
└── amount: double    // BTC value
```

### Block Structure

```
Block
├── block_id: string              // Unique block identifier
├── transactions: vector          // Transactions included in block
└── miner_id: string              // Address of miner who created block
```

---

## Transaction Validation

### Validation Checks (Executed in Order)

| # | Check | Purpose | Failure Message |
|---|-------|---------|-----------------|
| 1 | Input Existence | UTXO must exist and belong to sender | "Inputs do not exist" |
| 2 | No Duplicate Inputs | Same UTXO cannot be spent twice | "Duplicates exist in inputs" |
| 3 | Positive Outputs | All amounts must be > 0 | "Negative outputs exist" |
| 4 | Sufficient Funds | Sum(inputs) >= Sum(outputs) | "Value constraints failed" |
| 5 | Mempool Conflict | UTXOs not already in pending txs | "UTXOs already in mempool" |

### Security Features

- **Double-Spend Prevention (Same TX)**: Duplicate check catches using same UTXO twice
- **Double-Spend Prevention (Mempool)**: `spent_UTXOs` set tracks all committed UTXOs
- **Race Attack Prevention**: First-seen rule - first valid transaction wins
- **Ownership Verification**: `isBelongsTo()` confirms UTXO ownership

---

## Building & Running

### Prerequisites

- C++17 compatible compiler (g++ recommended)
- Standard library only (no external dependencies)

### Compilation

```bash
# Compile the main simulator
g++ -std=c++17 -o bitcoin_simulator src/main.cpp src/block.cpp src/mempool.cpp \
    src/transaction.cpp src/utxo_manager.cpp src/validator.cpp tests/test_scenarios.cpp

# Or compile tests separately
g++ -std=c++17 -DTEST_MAIN -o test_runner src/block.cpp src/mempool.cpp \
    src/transaction.cpp src/utxo_manager.cpp src/validator.cpp tests/test_scenarios.cpp
```

### Running

```bash
./bitcoin_simulator
```

---

## Usage Guide

### Main Menu Options

```
Main Menu:
 1. Create new transaction  - Create and validate a new transaction
 2. View UTXO set          - Display all unspent transaction outputs
 3. View mempool           - Show pending unconfirmed transactions
 4. Mine block             - Mine transactions from mempool into a block
 5. Run test scenarios     - Execute automated test suite
 6. Print Block chain      - Display all mined blocks
 7. Exit simulation        - End the program
```

### Creating a Transaction (Option 1)

```
1. Enter sender's user ID (e.g., "Alice")
2. View displayed available UTXOs for that user
3. Enter number of inputs to use
4. For each input:
   - Enter source transaction ID (e.g., "GENESIS")
   - Enter output index (e.g., 0)
5. Enter number of outputs
6. For each output:
   - Enter amount to send
   - Enter recipient ID
```

### Mining a Block (Option 4)

```
1. Enter miner's address (who receives fees)
2. Enter number of transactions to include
3. System automatically:
   - Selects highest-fee transactions
   - Updates UTXO set
   - Creates coinbase reward for miner
   - Adds block to blockchain
```

---

## Test Scenarios

The test suite (`tests/test_scenarios.cpp`) covers 10 comprehensive scenarios:

| Test | Description | Expected Result |
|------|-------------|-----------------|
| 1 | Basic Valid Transaction | Accept |
| 2 | Multiple Inputs | Accept |
| 3 | Double-Spend in Same TX | Reject |
| 4 | Mempool Double-Spend | TX1 Accept, TX2 Reject |
| 5 | Insufficient Funds | Reject |
| 6 | Negative Amount | Reject |
| 7 | Zero Fee Transaction | Accept |
| 8 | Race Attack Simulation | First TX wins |
| 9 | Complete Mining Flow | Full cycle validation |
| 10 | Unconfirmed Chain Spending | Reject |

### Genesis Block (Initial State)

```
- Alice   : 50.0 BTC (GENESIS, index 0)
- Bob     : 30.0 BTC (GENESIS, index 1)
- Charlie : 20.0 BTC (GENESIS, index 2)
- David   : 10.0 BTC (GENESIS, index 3)
- Eve     :  5.0 BTC (GENESIS, index 4)
```

---

## Design Decisions Summary

| Decision | Rationale |
|----------|-----------|
| **No Unconfirmed Chaining** | Transactions can only spend confirmed UTXOs - simpler to validate and prevents complex dependency chains |
| **Mempool Capacity Limit** | Demonstrates fee-based eviction when mempool is full |
| **First-Seen Rule** | Prevents Replace-By-Fee (RBF) attacks - simulates original Bitcoin behavior |
| **Fee = Input - Output** | Standard Bitcoin fee calculation - difference goes to miner |
| **Auto-generated TX IDs** | Simplifies demo - real Bitcoin uses hash of transaction |

---

## File Structure

```
CS216-Malicious_Nodes-UTXO_Simulator/
├── src/
│   ├── main.cpp          # Entry point, menu handling
│   ├── block.cpp/.h      # Block, Blockchain, mining logic
│   ├── mempool.cpp/.h    # Transaction pool management
│   ├── transaction.cpp/.h # Transaction data structures
│   ├── utxo_manager.cpp/.h # UTXO set management
│   ├── validator.cpp/.h  # Transaction validation
│   └── utils.h           # Helper functions
├── tests/
│   └── test_scenarios.cpp # Automated test suite
├── README.md             # This file
└── requirements.txt      # Build requirements
```

---

## License

This project is developed for educational purposes as part of CS-216: Introduction to Blockchain course.
