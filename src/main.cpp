#include "utxo_manager.h"

int main() {
    cout << "Testing UTXO Manager Feature..." << endl;
    
    UTXO_manager manager;

    // 1. Add some initial coins (Genesis block style)
    cout << "Adding UTXOs..." << endl;
    manager.add_utxo("tx_genesis", "Alice", 50.0, 0);
    manager.add_utxo("tx_genesis", "Bob", 30.0, 1);

    // 2. Test Balance
    cout << "Alice's Balance (Expected 50.0): " << manager.get_balance("Alice") << endl;
    cout << "Bob's Balance (Expected 30.0): "   << manager.get_balance("Bob") << endl;

    // 3. Test Spending (Removal)
    cout << "Removing Alice's UTXO..." << endl;
    manager.remove_utxo("tx_genesis", 0);

    // 4. Verify Removal
    if(manager.is_unspent("tx_genesis", 0)) {
        cout << "Error: UTXO should be gone!" << endl;
    } else {
        cout << "Success: UTXO removed correctly." << endl;
    }

    cout << "Alice's New Balance (Expected 0.0): " << manager.get_balance("Alice") << endl;

    return 0;
}